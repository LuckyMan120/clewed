alter table maenna_about
    add video_url VARCHAR(300) null;

alter table maenna_company
    add started_fundraising tinyint(4) DEFAULT 0;

alter table maenna_company
    add flags_status text DEFAULT null;