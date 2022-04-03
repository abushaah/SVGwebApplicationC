delete from FILE where file_name='Emoji_Hearts.svg';
delete from FILE where file_name='Emoji_Butterfly.svg';
/* Will clear rest of rows from other tables, since they are connected by ID */
truncate table DOWNLOAD;
truncate table MODIFICATION;
truncate table FILE;
