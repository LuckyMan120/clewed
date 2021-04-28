INSERT INTO maenna_sections (sectionid, type, parentid, maenna_name, title)
VALUES (85, 'mtab', 3, 'share_knowledge', 'Discuss project');

INSERT INTO maenna_sections (sectionid, type, parentid, maenna_name, title, maenna_panel, position)
VALUES(86, 'section', 85, 'maenna_share_knowledge', 'Discuss project content', 'multi', 'middle');

INSERT INTO maenna_sections (sectionid, type, parentid, maenna_name, title, position, weight)
VALUES(87, 'section', 85, 'connections', 'Connections', 'left', 3);

INSERT INTO maenna_sections (sectionid, type, parentid, maenna_name, title, position, weight)
VALUES(88, 'section', 85, 'collaborators', 'Requests', 'left', 4);
