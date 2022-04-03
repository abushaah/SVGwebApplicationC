insert into FILE (file_name, file_title, file_description, creation_time, file_size) VALUES ('Emoji_Hearts.svg', 'Heart', 'Two Pink Hearts', CURRENT_TIMESTAMP, '12');
insert into FILE (file_name, file_title, file_description, creation_time, file_size) VALUES ('Emoji_Butterfly.svg', 'Blue Butterfly', null, CURRENT_TIMESTAMP, '26');
insert into MODIFICATION (mod_type, mod_summary, mod_date, svg_id) VALUES ('change attribute', 'change fill to blue for path 1', CURRENT_TIMESTAMP, (select svg_id from FILE where file_name='Emoji_Hearts.svg'));
insert into MODIFICATION (mod_type, mod_summary, mod_date, svg_id) VALUES ('add attribute', 'add circle at 2,3 with radius 6', CURRENT_TIMESTAMP, (select svg_id from FILE where file_name='Emoji_Butterfly.svg'));
insert into DOWNLOAD (d_time, d_hostname, svg_id) VALUES (CURRENT_TIMESTAMP, @@hostname, (select svg_id from FILE where file_name='Emoji_Hearts.svg'));
insert into DOWNLOAD (d_time, d_hostname, svg_id) VALUES (CURRENT_TIMESTAMP, @@hostname, (select svg_id from FILE where file_name='Emoji_Butterfly.svg'));
