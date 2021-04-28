<?php
global $user;

function calculateCheckboxPercent($checkboxes) {
  if (empty($checkboxes)) {
    return false;
  }

  $count = count($checkboxes);
  $checked = 0;
  foreach($checkboxes as $checkbox){
    if($checkbox) $checked++;
  }
  $percent = floatval($checked/($count/100));
  return array($checked, $count, $percent);
}

function explore_capital_raising()
{
    global $user, $AccessObj;
    //if ($_REQUEST['id'] != $user->uid) return '';

    // Tab
    $tab = get_var('tab');

    // User ID
    $company_id = get_var('id');

    // Panel name (to render the view)
    $panel = get_var('panel');

    $step1_url = 'account?tab=companies&page=company_detail&mtab=about&id=' . $company_id . '&view=edit&panel=edit_capital_raising_info_step1';
    $step2_url = 'account?tab=companies&page=company_detail&mtab=about&id=' . $company_id . '&view=edit&panel=edit_capital_raising_info_step2';
    $url_analysis = "/account?tab=companies&page=company_detail&id={$company_id}&mtab=analysis";

    if ($tab == 'companies') {
        $selectedOptions = getSelectedCompanyPage2($company_id);
        $flags_statuses = getCompanyFlagsStatuses($company_id);
        if ($flags_statuses["capital"][1] == 'saved')
            $step = 1;
        else $step = 2;
        $companyCheckboxes = getCapitalRaisingCheckboxes($company_id, $step);
        $companyCheckboxInfo = calculateCheckboxPercent($companyCheckboxes);
        $companyCheckboxPercent = $companyCheckboxInfo[2];

//        if ($companyCheckboxPercent == 100) {
//            $step_url = $step2_url;
//        }

        $adminCheckboxes = getFundraisingAdminCheckboxes($company_id);
        // Get first 7 options only
        $adminCheckboxes7 = $adminCheckboxes;
        unset($adminCheckboxes7['turn_on_relevant_pages']);
        unset($adminCheckboxes7['fundraising_declined']);
        unset($adminCheckboxes7['fundraising_approved']);

        $adminPercentInfo = calculateCheckboxPercent($adminCheckboxes7);
        $adminPercent = number_format($adminPercentInfo[2]);

        $status = '';
        $percent = false;
        $sql = "select public FROM maenna_company where companyid = %d";
        $result = db_query($sql, array($_REQUEST['id']));
        $Row = db_fetch_array($result);

        if ($Row['public'] != '0' && $adminCheckboxes['fundraising_approved']) {
            if ($selectedOptions['fundraising']) {
                $label = 'Fundraising launched';
                $status = '';
            }
            else {
                $label = 'Fundraising approved';
                $status = 'Preparing for Marketing';
            }

            $percent = 100;
            $step_url = "";
        } else if ($adminCheckboxes['fundraising_declined']) {
            $label = 'Fundraising Declined';
            $status = "Details in <a href={$url_analysis} style='color: #00a2bf'>Analysis</a> tab";
            $percent = 0;
            $step_url = $url_analysis;
        } else if ($adminPercent > 0) {
            $label = 'Fundraising in review';
            $status = $adminPercent . '% reviewed';
            $percent = $adminPercent;
            $step_url = $url_analysis;
        } else {
            $percent = $companyCheckboxPercent;
            if ($step == 1) {

                $step_url = $step1_url;
                if (array_sum($companyCheckboxes) == 0) $label = 'Explore Capital Raising';
                else $label = 'Step 1 in progress';
                if ($companyCheckboxPercent == 100)
                    $status = 'Click Submit to progress to step 2';
                else
                    $status = ($companyCheckboxPercent <= 50 ? 'Likely not a fit, progress to step 2' : 'Likely fit, progress to step 2');
            } else if ($step == 2) {
                $step_url = $step2_url;
                if ($companyCheckboxPercent == 100 && $flags_statuses["capital"][2] == 'submitted') {
                    $status = "Waiting for admin to start review";
                    $label = 'Fundraising Applied';
                    $step_url = "";
                } else {
                    $label = 'Step 2 in progress';
                    $step_url = $step2_url;
                    if ($companyCheckboxPercent == 100)
                        $status = "Click submit to apply fundraising";
                    else
                        $status = number_format($companyCheckboxPercent) . '% of step 2 screening completed';
                }
            }
        }
            $explore_capital_raising_events = array(
                array(
                    'value' => '<div class="sidebar_box status_bars_block" >',
                    'type' => 'html',
                ),
                array(
                    'prefix' => '<div class="blue_button_box" style="margin-top: 15px;">',
                    'label' => $label,
                    'link' => $step_url,
                    'type' => 'button',
                    'suffix' => '</div>'
                ),
                array(
                    'prefix' => '<div class="capital_rising_block"><div class="status_bars_wrapper">',
                    'link' => $step_url,
                    'type' => 'statusbar_without_checkbox',
                    'label' => $label,
                    'status' => $status,
                    'percent' => $percent,
                    'suffix' => '</div></div>'
                ),
                array(
                    'value' => '</div>',
                    'type' => 'html',
                ),
//                 array(
//                     'value' => fundraising_additional_info($selectedOptions),
//                     'type' => 'html',
//                 ),
                array(
                    'type' => 'path_to_launch'
                ),
            );
        }

        $output = render_explore_capital_raising($explore_capital_raising_events, $step);
        $output.= fundraising_additional_info($selectedOptions);

        return $output;
}


function render_explore_capital_raising($events){
        global $user, $AccessObj;
        // User ID
        $company_id = get_var('id');

        // Panel name (to render the view)
        $panel = get_var('panel');

        $path_to_launch_class = '';
        if (strpos($panel, 'edit_capital_raising_info_step') !== false) {
            $path_to_launch_class = ' edit_capital_raising_info_step';
        }

        //check if professional user connected to this project returned obj
        $isMatchProfInProject = getActiveConnection($company_id,$AccessObj->uid);

        if ('people' == $AccessObj->user_type && $user->uid != $company_id && !$isMatchProfInProject) {
            return '';
        }

        $output = "";
        if (!$events || !is_array($events) || count($events) == 0) {
            return FALSE;
        }

        foreach ($events as $event) {
            if (isset($event['prefix'])) {
                $output .= $event['prefix'];
            }

            switch ($event["type"]) {
                case 'button':
                    $link = ($event['link'] != "") ? "href='{$event['link']}'" : "href='#' onclick='return false;'";
                    $output .= "<a class='blue_button' {$link}>{$event['label']}</a>";
                    break;

                case 'html':
                    $output .= $event['value'];
                    break;

                case 'title':
                    $output .= "<div class=\"wrapper\"><div class=\"box_title\"><span data-tooltip=\"" . $event['tooltip'] . "\" style=\"font-size:14px;text-transform:capitalize !important;cursor: pointer;\">" . $event['label'] . "</span></div></div>";
                    break;

                case 'statusbar_with_checkbox':
                    if ($event['callback'] == 'get_referral_status') {
                        $js_fun = 'class = "referral_code"';
                    } else {
                        $js_fun = '';
                    }

                    $value = call_user_func($event['callback']);

                    $tooltip_message = '';
                    if ($event['tooltip_callback']) {
                        $tooltip_message = call_user_func($event['tooltip_callback'], $value);
                    }

                    $output .= '<a ' . $js_fun . ' href="' . $event['link'] . '"' . ($tooltip_message ? " data-tooltip='$tooltip_message'" : '') . '><div class="get-started-event">' . $event['label'] . '</div>';
                    $output .= '<div id="get-started-bar-wrapper"><div class="get-started-bar"></div>';
                    $output .= '<div class="get-started-progress" style="width: ' . $value . '%"></div></div>';
                    $output .= '<div class="get-started-check-box ' . check_mark($value) . '"></div><div class="clear-fix"></div>';
                    $output .= '</a>';

                    break;

                case 'statusbar_without_checkbox':
                    $link = ($event['link'] != "") ? "href='{$event['link']}'" : "href='#' onclick='return false;'";
                    $output .= '<a ' . $link . (isset($event['tooltip_message']) ? " data-tooltip='$tooltip_message'" : '') . '>';
                    $output .= '<div class="status-bar-wrapper"><div class="status-bar"></div>';
                    $output .= '<div class="status-bar-progress" style="width: ' . $event['percent'] . '%"></div></div>';
                    $output .= '<div class="status-bar-event">' . $event['status'] . '</div>';
                    $output .= '<div class="clear-fix"></div>';
                    $output .= '</a>';
                    break;

                case 'path_to_launch':
                    $output .= '<div class="sidebar_box path_to_launch' . $path_to_launch_class . '">';
                    $box_title = '<a data-tooltip="Click to open" data-toggle="modal" data-title="Path to fundraising" href="#fundraising_path_dialog" style="display:block;color:#929497;margin-top:18px;">Path to fundraising <span class="collapse-icon" style="float: right">+</span></a>';

                    // Box name
                    $output .= '<div class="block_title">' . $box_title . '</div>';
                    
                    $output .= '<div id="fundraising_path_dialog" class="dialog-wrapper" style="display:none;">
            		  <div style="padding:20px;">
                        <style>
                            /*h4 {font-size:130% !important;margin-top:10px;}
                            a {color:#00a2bf !important;}*/
                            #fundraising_path{font-size:16px;}
                            #fundraising_path h4{font-size:18px;margin:13px 0 8px 0;font-family:LatoRegular;}
                        </style>';
                    // $output .= '<div class="wrapper" style="margin-top:5px;">';

                    $link_screening_step1 = '/account?tab=companies&page=company_detail&mtab=about&id=' . $company_id . '&view=edit&panel=edit_capital_raising_info_step1';
                    $link_screening_step2 = '/account?tab=companies&page=company_detail&mtab=about&id=' . $company_id . '&view=edit&panel=edit_capital_raising_info_step2';
                    // Data list
                    $list = [
                        'screening' => [
                            'heading' => 'Screening',
                            'tooltip' => 'You may fail step 1 screening and pass in step 2 in rare cases. Beyond screening, your information in step 2 is key to prepare analyst research, organize investor package, insights and marketing events to build confidence and succeed',
                            'body' => '<a href="' . $link_screening_step1 . '" class="blue-link">General screening</a> requires ≥ 75% pass<br><a href="' . $link_screening_step2 . '" class="blue-link">Detailed data submission</a> is required for diligence and analysis',
                        ],
                        'approved/declined' => [
                            'heading' => 'Approved/Declined',
                            'tooltip' => 'We prepare analyst research, organize your KPIs, MD&A and related files for investor diligence, PR, Insight discussions and marketing.  You build your list of vendors, customers, and network. If declined, your research sets your improvement implementation for future raise.',
                            'body' => 'Prepare for marketing or to implement improvements for future fundraising',
                        ],
                        'launch' => [
                            'heading' => 'Launch',
                            'tooltip' => 'Execute your marketing plan with a target to get >30% of your raise in your soft launch within your network. Clewed and its network invest in select opportunities they can diligence & monitor on our platform.',
                            'body' => 'Execute marketing plan. Hire marketing expertise & execute a well planned marketing plan.Target to fill more than 10% of your offering from  own network before opening broadly and before potential Clewed participation',
                        ],
                        'close' => [
                            'heading' => 'Close',
                            'body' => 'Funds held in escrow and released at closing',
                        ],
                        'monitoring' => [
                            'heading' => 'Monitoring',
                            'body' => 'On going owner-friendly performance reporting and monitoring on Clewed',
                        ],
                    ];

                    // Output items
                    $output .= '<div id="fundraising_path" class="list-paths">';
                    foreach ($list as $key => $item) {
                        $output .= '<div class="list-path--item ' . $key . '">';

                        $className = 'item-title';
                        $partTooltip = '';
                        $partIcon = '';
                        if (!empty($item['tooltip'])) {
                            $partTooltip = ' data-tooltip="' . $item['tooltip'] . '"';
                            $partIcon = ' <img src="/themes/maennaco/images/questionmark.png">';
                        }
                        $output .= "<h4 class='$className'$partTooltip><span class='title'>${item['heading']}</span>$partIcon</h4>";
                        $output .= '<div class="item-body">' . $item['body'] . '</div>';
                        $output .= '</div>';
                    }
                    $output .= '</div>';
                    $output .= '</div>';
                    // $output .= '</div>';
                    
                    $output .= '</div>
                    </div>';
                    
                    break;

                default:
                    break;
            }

            if (isset($event['suffix'])) {
                $output .= $event['suffix'];
            }
        }

        return $output;
}
