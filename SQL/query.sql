/* 1 (required) Display all columns from all files, sorted by file name */
SELECT * FROM FILE order by file_name;

/* 2 (required) Display all columns of all modifications, sorted by modification date (most recent first) */
SELECT * FROM MODIFICATION order by mod_date;

/* 3 (required) Display names, sizes, and modification dates of all files modified between specific dates. You 
can hard-code specific start / end dates into your SQL code, but note that we may change 
these dates when we grade your code. The result must be sorted by file size. */
SELECT file_name, file_size, mod_date FROM FILE, MODIFICATION where '2022-04-01 01:47:17' <= creation_time AND creation_time <= '2022-04-07 01:47:17' order by file_size; /* where the first date is the start date and the second date is the end date*/

/* 4 (required) Display the name and download date of the most recently downloaded file. */
SELECT file_name, d_time FROM FILE, DOWNLOAD ORDER BY d_time DESC LIMIT 1;

/* 5 (additional non-simplistic queries)
Purpose: to get the file size statistics for download purposes
Query: displays all files and their sizes, last 4 rows collect and display statistics about the file sizes, such as the sum, average, max, and min.
Uses: outer join / full join, aggregate functions */
(SELECT file_name, file_size FROM FILE) UNION (SELECT 'SUM' AS file_name, SUM(file_size) file_size FROM FILE) UNION (SELECT 'AVERAGE' AS file_name, AVG(file_size) file_size FROM FILE) UNION (SELECT 'MAX' AS file_name, MAX(file_size) file_size FROM FILE) UNION (SELECT 'MIN' AS file_name, MIN(file_size) file_size FROM FILE);

/* 6 (additional non-simplistic queries)
Purpose: to find popular / most interacted with files
Query: lists all the files with their matching date data if and only if they have been downloaded OR modified,
       to track which files have been edited OR needed most.
       It requires data from all 3 tables and if there is an entry in the FILE table with no modification OR
       download, then it will not be displayed.
       The result is ordered by creation time, and only the first modification/edit date is printed, no duplicate download or modification dates.
Uses: inner join */
SELECT file_name, creation_time, d_time, mod_date FROM FILE, DOWNLOAD, MODIFICATION WHERE (FILE.svg_id = DOWNLOAD.svg_id || FILE.svg_id = MODIFICATION.svg_id) GROUP BY file_name ORDER BY creation_time;

/*
Purpose: to identify files that have been edited more than once
Query: displays the file names that have been edited more than once
Uses: nested queries
*/
SELECT file_name FROM FILE WHERE svg_id = (SELECT svg_id FROM MODIFICATION GROUP BY svg_id HAVING COUNT(*) > 1);
