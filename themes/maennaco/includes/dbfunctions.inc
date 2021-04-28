<?php
function get_company_row($companyid)
{
    if(empty($companyid)) return false;
    $sql = "select * from maenna_company where companyid = %d";
    return db_query($sql, array($companyid));
}
function getCompanyFianacial($companyid)
{
    if(empty($companyid)) return false;
    $sql = "select * from maenna_company_data where companyid = %d and (data_type = 'earning' or data_type = 'revenue')";
    return db_query($sql, array($companyid));
}
/* EOF */
