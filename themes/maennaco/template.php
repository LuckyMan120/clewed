<?php
// $Id: template.php,v 1.16.2.3 2010/05/11 09:41:22 goba Exp $
/**
 * Sets the body-tag class attribute.
 * Adds 'sidebar-left', 'sidebar-right' or 'sidebars' classes as needed.
 */
function phptemplate_body_class($left, $right) {
    if ($left != '' && $right != '') {
        $class = 'sidebars';
    } else {
        if ($left != '') {
            $class = 'sidebar-left';
        }
        if ($right != '') {
            $class = 'sidebar-right';
        }
    }
    if ($_GET['tab'] == 'companies' && $_GET['page'] == 'company_detail') {
        if (isset($class)) {
            $class .= ' companies';
        } else {
            $class = 'companies';
        }
    }
    if ($_GET['tab'] == 'professionals' && $_GET['page'] == 'pro_detail') {
        if (isset($class)) {
            $class .= ' professionals';
        } else {
            $class = 'professionals';
        }
    }
    if ($_GET['tab'] == 'insights' || $_GET['tab'] == 'services' ) {
        if (isset($class)) {
            $class .= ' insights';
        } else {
            $class = 'insights';
        }
    }
    if (isset($class)) {
        print ' class="' . $class . '"';
    }
}

/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 *
 * @return a string containing the breadcrumb output.
 */
function phptemplate_breadcrumb($breadcrumb) {
    if (!empty($breadcrumb)) {
        return '<div class="breadcrumb">' . implode(' › ', $breadcrumb) . '</div>';
    }
}

/**
 * Override or insert PHPTemplate variables into the templates.
 */
function phptemplate_preprocess_page(&$vars) {
    $vars['tabs2'] = menu_secondary_local_tasks();
    // Hook into color.module
    if (module_exists('color')) {
        _color_page_alter($vars);
    }
// Add per content type pages
#
    if (isset($vars['node'])) {
# // Add template naming suggestion. It should alway use hyphens.
#// If node type is "custom_news", it will pickup "page-custom-news.tpl.php".
#
        $vars['template_files'][] = 'page-' . str_replace('_', '-', $vars['node']->type);
    }
    drupal_add_js('sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce.js');
    drupal_add_js('sites/all/libraries/ckeditor5/ckeditor.js');
    $vars['scripts'] = drupal_get_js();
}

/**
 * Add a "Comments" heading above comments except on forum pages.
 */
function maennaco_preprocess_comment_wrapper(&$vars) {
    if ($vars['content'] && $vars['node']->type != 'forum') {
        $vars['content'] = '<h2 class="comments">' . t('Comments') . '</h2>' . $vars['content'];
    }
}

/**
 * Returns the rendered local tasks. The default implementation renders
 * them as tabs. Overridden to split the secondary tasks.
 *
 * @ingroup themeable
 */
function phptemplate_menu_local_tasks() {
    return menu_primary_local_tasks();
}

/**
 * Returns the themed submitted-by string for the comment.
 */
function phptemplate_comment_submitted($comment) {
    return t(
        '!datetime — !username',
        array(
            '!username' => theme('username', $comment),
            '!datetime' => format_date($comment->timestamp)
        )
    );
}

/**
 * Returns the themed submitted-by string for the node.
 */
function phptemplate_node_submitted($node) {
    return t(
        '!datetime — !username',
        array(
            '!username' => theme('username', $node),
            '!datetime' => format_date($node->created),
        )
    );
}

/**
 * Generates IE CSS links for LTR and RTL languages.
 */
function phptemplate_get_ie_styles() {
    global $language;
    $iecss = '<link type="text/css" rel="stylesheet" media="all" href="' . base_path() . path_to_theme() . '/fix-ie.css" />';
    if ($language->direction == LANGUAGE_RTL) {
        $iecss .= '<style type="text/css" media="all">@import "' . base_path() . path_to_theme() . '/fix-ie-rtl.css";</style>';
    }
    return $iecss;
}

function maennaco_theme() {
    return array(
        "protypes_checkboxes" => array(
            'arguments' => array('form' => null),
        ),
        /*              "maenna_forms_pro_form" => array(
                       'arguments' => array('form' => NULL, 'theme' => $theme),
                       'template' => 'maenna_forms_pro_form',
                     )*/
    );
}
/*
function maennaco_protypes_checkboxes($form)
{
  $output = '<table cellspacing=0 cellpadding=0 border=0><tr><td>I am</td>';
  
  
  
  
  echo "<pre>";
  foreach($form as $key => $index)
  {
    echo "$key = $index\n";
  }
  echo "</pre>";
  
  return $output;
}
*/

/*
// modify theme api function
function maennaco_checkboxes($element)
{
  $class = 'form-checkboxes';
  if (isset($element['#attributes']['class'])) {
    $class .= ' ' . $element['#attributes']['class'];
  }
  $element['#children'] = '<div class="' . $class . '">' . (!empty($element['#children']) ? $element['#children'] : '') . '</div>';
  if ($element['#title'] || $element['#description']) {
    unset($element['#id']);
    return theme('form_element', $element, $element['#children']);
  }
  else {
    return $element['#children'];
  }
}
*/


/* EOF */
