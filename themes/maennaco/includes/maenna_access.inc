<?php

class Maenna_access
{
    var $user_type, $role_id, $uid, $Objects, $Access, $Sections,$cmp_type,$cmp_role,$coll_companies,$assigned_users,$firstname,$lastname,$canJoinInsight,$code_to_refer;
    var $Permission, $section_access;
    var $ratings = array();
    var $All_admins, $prv,$mtab;
    function __construct()
    {
        global $user;
        require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
        $this->Objects = $this->Obj_info = $this->Access = $this->Sections = $this->coll_companies = $this->assigned_users = array();
        
        $this->uid = $user->uid;

        $User_roles_key = array_keys($user->roles);
        $maenna_user_type = '';
        if(in_array(3, $User_roles_key)){
            $this->user_type = 'company';
            $this->role_id = 3;
        }elseif(in_array( 4, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 4;
        }elseif(in_array(5, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 5;
        }elseif(in_array(7, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 7;
        }elseif(in_array(6, $User_roles_key)){
            $this->user_type = 'admin';
            $this->role_id = 6;
        }elseif(in_array(8, $User_roles_key)){
            $this->user_type = 'investor';
            $this->role_id = 8;
        }elseif(in_array(10, $User_roles_key)){
            $this->user_type = 'super';
            $this->role_id = 10;
        }
        elseif(in_array(11, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 11;
        }
        elseif(in_array(12, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 12;
        }
        elseif(in_array(13, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 13;
        }
        elseif(in_array(14, $User_roles_key)){
            $this->user_type = 'people';
            $this->role_id = 14;
        }
        elseif(in_array(15, $User_roles_key)){
            $this->user_type = 'sales';
            $this->role_id = 15;
        }
        if($this->user_type == 'super')
        {
            $All_admins = Mae_users::all_admins();
            
            foreach($All_admins as $Admin)
            {
                $admin_id = $Admin['uid'];
                $name = ucwords($Admin['first_name'] . ' ' . $Admin['last_name']);
                $this->All_admins["$admin_id"] = $name;
                
            }
        }
        if ($this->user_type == 'admin') {
            $sql = "SELECT target_uid FROM maenna_connections WHERE assignee_uid = %d and conntype = '%s' and status = '%s'";
            $ass_res = db_query($sql, array($this->uid,'admin','active'));
            while($Row = db_fetch_object($ass_res)) {
                $this->assigned_users[] = $Row->target_uid;

            }

        }
        
        if ($this->user_type == 'company') {

// Set company type and role
			$cmpqry = "SELECT membership,company_type,projname FROM maenna_company WHERE companyid = %d";
			$cmp_type_res = db_query($cmpqry, array($user->uid));
			$cmp_type_tmp = db_fetch_object($cmp_type_res);
			$this->cmp_type = $cmp_type_tmp->membership;
			$this->cmp_role = $cmp_type_tmp->company_type;
            $this->firstname = $cmp_type_tmp->projname;
}

        if ($this->user_type == 'people') {
            $this->collaboration_companies();

            $sql = "select firstname,lastname,code_to_refer from maenna_people WHERE pid = %d";
            $result = db_query($sql,array($user->uid));
            $Row = db_fetch_object($result);
            $this->firstname = $Row->firstname;
            $this->lastname = $Row->lastname;
            $this->ratings = $this->user_rating();
            $this->code_to_refer = $Row->code_to_refer;
        }





        /*
        if(! isset($_SESSION['prv']))
        {
            $_SESSION['prv']  = array();
        }
        
        $this->prv = '';
        
        $no_history = sget($_REQUEST, 'update_section');
        $no_history .= sget($_REQUEST, 'submit');
        
        if(sget($_REQUEST, 'prv'))
        {
            array_shift($_SESSION['prv']);
            $this->prv = rebuild_url(array('tab','page','mtab','section','panel','target','id','uid','qid','aid','view'), $_SESSION['prv'][0]). "&prv=1";
        }else
        {
            $this->prv = rebuild_url(array('tab','page','mtab','section','panel','target','id','uid','qid','aid','view'), $_SESSION['prv'][0]) . "&prv=1";
            $str_request = serialize($_REQUEST);
            $str_prv = '';
            if(count($_SESSION['prv'])>0)
            {
                $str_prv = serialize($_SESSION['prv'][0]);
            }
            if(($str_prv != $str_request) && $no_history == '' ) array_unshift($_SESSION['prv'], $_REQUEST);
        }
        if(count($_SESSION['prv']) > 10)
        {
            array_pop($_SESSION['prv']);
        }
        */
        
        
    }

    function _authz()
    {
        require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
        if(in_array($this->user_type, array('super'))) {
            $this->grant_all();
            return;
        }

        //sub user groups, possible value = au_assign ..... in Mae_users class
        $relation_type = Connections::_relation();
        $this->_relation = $relation_type;
//        var_dump( $relation_type);
        if( ! $relation_type)
        {
            $this->deny_all();
            return;
        }
        $Perm = $this->load_permission($relation_type);
        //test_array($Perm);
        
        $this->apply_permission($Perm);
        //test_array($this->Com_sections);
    }
    
    function grant_all()
    {
        foreach($this->Com_sections as $tab_name => &$Tab_data)
        {
            $Tab_data['access'] = 'write';
            foreach($Tab_data['sections'] as $section_name => &$Section_data)
            {
                $Section_data['access'] = 'write';
            }
        }
    }
    function deny_all()
    {
        foreach($this->Com_sections as $tab_name => &$Tab_data)
        {
            $Tab_data['access'] = 'hide';
            foreach($Tab_data['sections'] as $section_name => &$Section_data)
            {
                $Section_data['access'] = 'hide';
            }
        }
    }
    function apply_permission($Perm)
    {
        //test_array($this->Com_sections);   
        foreach($this->Com_sections as $tab_name => &$Tab_data)
        {
            $tab_access = 'hide';
            foreach($Tab_data['sections'] as $section_name => &$Section_data)
            {
                $section_id = $Section_data['id'];
                $Section_data['access'] = $Perm["$section_id"];
                if($Perm["$section_id"] != 'hide')$tab_access = 'read';
            }
            $Tab_data['access'] = $tab_access;
        }
    }
    
    function load_permission($usergroup_subtype)
    {
        $Temp = array();
        if(empty($usergroup_subtype)) {return false;}
        $companyid = sget($_REQUEST, 'id');
        //echo $companyid;
        $sql = "select * from maenna_access where editor_utype = 'super'";
        $result = db_query($sql);
        while($Row = db_fetch_object($result))
        {
            $Settings = unserialize($Row->settings);
            $Temp = array_merge_recursive($Temp, $Settings);
        }
        //test_array($Temp);
        if($companyid)
        {
            $sql = "select * from maenna_access where  companyid = %d";
            $result = db_query($sql, array($companyid));
            $Row = db_fetch_object($result);
            if($Row)
            {
                $Temp_com = unserialize($Row->settings);
                
                $Temp = MergeArrays($Temp,$Temp_com);
               
            }
        }
           //die($usergroup_subtype);
        $tmp_Perm = $Temp["$usergroup_subtype"];
        //test_array($tmp_Perm);
        // Now we want to test each node for permission and change it if the user is collaborator and collaborator permissions are higher
        if (in_array($companyid,$this->coll_companies) && $usergroup_subtype != 'pu_collaborators') {

            $Temp_coll = array();
            $sql1 = "select * from maenna_access where editor_utype = 'super'";
            $result1 = db_query($sql1);
            while($Row1 = db_fetch_object($result1))
            {
                $Settings_coll = unserialize($Row1->settings);
                $Temp_coll = array_merge_recursive($Temp_coll, $Settings_coll);
            }
            //test_array($Temp_coll);

            $coll_Perm = $Temp_coll['pu1_collaborators'];
            //test_array($coll_Perm);

/*            foreach ($tmp_Perm as $key1 => $value3) {

                echo $key1."=>".$value3." ----- ".$key1,"=>".$coll_Perm[$key1]."<br>";

            }
            die();*/

            //All possible permissions higher then hide
            $perm_1st_level = array('read','write','download','all','view','edit');
            //All possible permissions higher then read or view
            $perm_2nd_level = array('write','download','all','edit');

                foreach($tmp_Perm as $key => $value) {
                   if ($value == 'hide' && in_array($coll_Perm[$key],$perm_1st_level))

                        $tmp_Perm[$key] = $coll_Perm[$key];
                    elseif (($value == 'read' || $value == 'view') && in_array($coll_Perm[$key],$perm_2nd_level))
                        $tmp_Perm[$key] = $coll_Perm[$key];
                    //$tmp_Perm[$key] = 'damjan';
                }


        }

        //test_array($tmp_Perm);
        //die();
        return $tmp_Perm;
    }
    
    function section_mode($section_name)
    {
        //echo $section_name;
        $mtab = sget($_REQUEST, 'mtab');
        //test_array($this->Com_sections );
        if(empty($mtab))
        {
            foreach($this->Com_sections as $tab_name => $Tab_data)
            {
                if($Tab_data['access'] != 'hide')
                {
                    $mtab = $tab_name;
                    break;
                }
            }
        }
        //echo $section_name;
        
        // all sections in com_details page has associate setting values.
        // if one section has empty access setting, then its out of the scope of the permissions setting table. hence its access is not being controlled!!
        $url_section = sget($_REQUEST, 'section');
        foreach($this->Com_sections as $tab_name => $Tab_data)
        {
            foreach($Tab_data['sections'] as $name => $Data)
            {
                
                if(strcasecmp($tab_name, $mtab) == 0)
                {
                    if( (strcasecmp($section_name, $name) == 0 ) ||
                        (   (strcasecmp($url_section, $name) == 0) &&
                            (strcasecmp($section_name, $Data['panel']) == 0 )
                        )
                      )
                    {
                        //echo "<br>1 $tab_name, 2 $mtab 3 $section_name 4 $name 5 $url_section ";
                        return $Data['access'];
                    }
                    
                }
            }
        }
        foreach($this->Com_sections['common']['sections'] as $Sec)
        {
            if( (strcasecmp($section_name, $name) == 0 ) ||
                    (   (strcasecmp($url_section, $name) == 0) &&
                        (strcasecmp($section_name, $Sec['panel']) == 0 )
                    )
                    )
               {
                   //echo "<br>x1 $tab_name, x2 $mtab x3 $section_name x4 $name x5 $url_section "; 
                    return $Sec['access'];
               }
        }

        if ($section_name == 'sections') {
            if($mtab == 'share_knowledge_details')
                return $this->Com_sections['share_knowledge']['sections']['maenna_share_knowledge']['access'];

            if ($mtab == 'share_knowledge' || $mtab == 'analysis' || $mtab == 'plan' || $mtab == 'file')
                return $this->Com_sections[$mtab]['sections']['maenna_'.$mtab]['access'];

            elseif ($mtab == 'advice') return $this->Com_sections[$mtab]['sections']['maenna_events']['access'];

            elseif ($mtab == 'performance') return $this->Com_sections[$mtab]['sections']['maenna_plan']['access'];

            else return  $this->Com_sections[$mtab]['sections'][$mtab]['access'];

        }

        if ($section_name == 'sortanalysis') {

            if ($mtab == 'analysis' || $mtab == 'plan')
                return $this->Com_sections[$mtab]['sections']['maenna_'.$mtab]['access'];

            else return  $this->Com_sections[$mtab]['sections'][$mtab]['access'];

        }
        if ($section_name == 'analysis_details') {

            if ($mtab == 'summary')
                return $this->Com_sections['analysis']['sections']['maenna_analysis']['access'];

        }
        if ($section_name == 'share_knowledge_details') {

            if ($mtab == 'share_knowledge_details')
                return $this->Com_sections['share_knowledge']['sections']['maenna_share_knowledge']['access'];

        }
        if ($mtab == 'share_knowledge_details' && ($section_name == 'connections' || $section_name == 'collaborators'))

                return $this->Com_sections['share_knowledge']['sections'][$section_name]['access'];
    
        return '';
    }
    function pro_sections_access()
    {
        require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
        $pro_id = sget($_REQUEST, 'id');
        if($this->user_type == 'super') return 'write';
        elseif($this->user_type == 'admin' && Connections::admin_of($pro_id) ) return 'write';
        elseif($this->user_type == 'people' && $this->uid == $pro_id) {
            return 'read';
        }
        elseif(Connections::pro_is_visible($pro_id)) return 'read';
        elseif(isApproved($pro_id)) return 'read';
        else return NULL;
    }
    function _writable($maenna_name)
    {
        
       return true;
    }
    
    function update_access($Param)
    {

        $DBKeys = array('accessid',
                        'usergroup',
                        'settings',
                        'editorid',
                        'editor_utype',
                        'companyid',
                        'access');
        $sql = "insert into maenna_access (".implode(',', $DBKeys).")
                values (%d, '%s','%s',%d,'%s',%d,'%s')
                on duplicate key update
                    settings = '%s',
                    editorid = %d,
                    editor_utype = '%s',
                    access = '%s'
                ";
        
        array_push($Param, $Param['settings'], $Param['editorid'], $Param['editor_utype'], $Param['access']);
        //($sql);
        //die(test_array($Param));
        return db_query($sql, $Param);
    }
    function remove_access($companyid)
    {
        if(empty($companyid)) return false;
        $sql = "delete from maenna_access where companyid = %d and usergroup = 'com' limit 1";
        return db_query($sql, array($companyid));
    }
    function admin_of($target_uid)
    {
        $sql = "select * from maenna_connections where target_uid = '%s' and assignee_uid = '%s' and status = 'active' and role = 'admin' ";
        $result = db_query($sql, array($target_uid, $this->uid));
        if($Row = db_fetch_object($result) !== false) return true;
        else return false;
    }
//CP DEV
    function collaboration_companies() {
    
        $sql = "select target_uid from maenna_connections where assignee_uid = '%s' and status = 'active' and conntype='collaborator'";
        $result = db_query($sql, array($this->uid));
        
        while ($Row = db_fetch_object($result)) {
        
            array_push($this->coll_companies,$Row->target_uid);
        
        }
    
    
    }

    function ifCompletedQuestionnaire() {
        $uid = $this->uid;

        if ($this->user_type == 'people') $target = 'pro';
        elseif ($this->user_type == 'company') $target = 'company';

        $sql1 = "select  recordid
                from    maenna_questionair
                where   status = 1 and
                        target='%s' and usertype= %d and
                        parentid is null
                order by recordid asc";
        $result1 = db_query($sql1,array($target,$this->role_id));
        $Row1 = db_fetch_object($result1);

        $questionnaire_id = $Row1->recordid;

        $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A
                        on Q.recordid = A.qid and A.uid = %d
                    where parentid = %d and ((type = 'question') or (type = 'fileuploader'))";
        $result = db_query($sql, array($uid, $questionnaire_id, $this->role_id));
        $completed = true;
        while($Row = db_fetch_object($result))
        {
            if(empty($Row->answer)) $completed = false;
        }
        return $completed;

    }

    function user_rating () {

        $sql = "select
                  count(*) total,
                  sum(case when rate_type >0 then 1 else 0 end) positive,
                  sum(case when rate_type = 0 then 1 else 0 end) negative,
                  avg(rate_type) rate
                from user_rating
                where target_uid = %d";
        $result = db_query($sql,$this->uid);
        $Row = db_fetch_object($result);
        return array('total'=>$Row->total,'positive'=>$Row->positive,'negative'=>$Row->negative, 'rate' =>$Row->rate);

    }

}


/* EOF */