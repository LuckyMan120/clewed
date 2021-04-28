<?php
//$maenna_page['right']= array('upload_box','maenna_selection', 'mgmt_topics');
function info_tools($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    $content = <<< END
    <div style='padding-left:15px;'>
        <div><a href='/account?a=information&tab=insights&panel=upload_box_panel'>Add Information</a></div>
        <div><a href='/account?a=information&tab=postings&panel=posting_panel&postingtype=council'>Post advisory council opening</a></div>
        <div><a href='/account?a=information&tab=postings&panel=posting_panel&postingtype=project'>Post a speical project</a></div>
        <div><a href='/account?a=information&tab=events&panel=event_posting&eventtype=advisory'>Post Advisory Council Event</a></div>
        <div><a href='/account?a=information&tab=events&panel=event_posting&eventtype=group'>Post Group Event</a></div>
        <div><a href=''>Ask a questoin</a></div>
        <div>&nbsp;</div>
        <div><a href='/account?a=information&tab=questionnaire&panel=questionnaire&target=company'>Company Questionnaire</a></div>
        <div><a href='/account?a=information&tab=questionnaire&panel=questionnaire&target=pro'>Professional Questionnaire</a></div>
        <div>&nbsp;</div>
        <div><a href=''>Refer a company</a></div>
        <div><a href=''>Refer a professional</a></div>
    </div>
END;
    $Block['body'] = sidebar_box($Block['title'],$content);
    return $Block;
}


/* EOF */