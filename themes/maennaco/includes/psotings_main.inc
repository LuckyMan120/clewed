<?php
//   $maenna_page['content'] = array('acp_list','project_list');
//

function acp_list($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $content =<<< END
    <div>
        <div style='margin-top:15px;font:bold 12px arial'>Advisory Council Posting</div>
        <table>
            <thead>
                <tr><td><b>Date</b></td>
                    <td><b>Time</b></td>
                    <td><b>Sector</b></td>
                    <td><b>Topic</b></td>
                    <td><b>Professional</b></td>
                    <td><b>Opening</b></td>
                    <td><b>Select</b></td>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td align=right colspan=7>more</td>
                </tr>
            </tbody>
        </table>
    </div>
    
END;
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function acp_list_panel($op = null){
    
}



function project_list($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $content =<<< END
    <div>
        <div style='font:bold 12px arial'>Special Projects Posting</div>
        <table>
            <thead>
                <tr><td><b>Date</b></td>
                    <td><b>Time</b></td>
                    <td><b>Sector</b></td>
                    <td><b>Topic</b></td>
                    <td><b>Professional</b></td>
                    <td><b>Opening</b></td>
                    <td><b>Select</b></td>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td align=right colspan=7>more</td>
                </tr>
            </tbody>
        </table>
    </div>
    
END;
        
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}
function project_list_panel($op = null){
    
}
/* EOF */