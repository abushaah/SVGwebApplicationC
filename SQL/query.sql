-- 1
SELECT * FROM FILE order by file_name;

-- 2
SELECT * FROM MODIFICATION order by mod_date;

-- 3
SELECT file_name, file_size, mod_date FROM FILE, MODIFICATION where '2022-04-01 01:47:17' <= creation_time AND creation_time <= '2022-04-07 01:47:17' order by file_size;
/* where the first date is the start date and the second date is the end date*/

-- 4
SELECT file_name, d_time FROM FILE, DOWNLOAD ORDER BY d_time DESC LIMIT 1;
