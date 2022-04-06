/* 1 (required) Display all columns from all files, sorted by file name */
SELECT * FROM FILE order by file_name;

/* 2 (required) Display all columns of all modifications, sorted by modification date (most recent first) */
SELECT * FROM MODIFICATION order by mod_date DESC;

/* 3 (required) Display names, sizes, and modification dates of all files modified between specific dates. You 
can hard-code specific start / end dates into your SQL code, but note that we may change 
these dates when we grade your code. The result must be sorted by file size. */
SELECT file_name, file_size, mod_date FROM (FILE INNER JOIN MODIFICATION ON FILE.svg_id=MODIFICATION.svg_id) WHERE mod_date >= '2022-02-01' and mod_date <= '2022-03-07';
/* where the first date is the start date and the second date is the end date*/

/* 4 (required) Display the name and download date of the most recently downloaded file. */
SELECT file_name, d_time FROM FILE, DOWNLOAD ORDER BY d_time DESC LIMIT 1;

/* 5 (additional non-simplistic queries)
Purpose: to get the file size statistics for download purposes
Query: displays all files and their sizes, last 4 rows collect and display statistics about the file sizes, such as the sum, average, max, and min.
Uses: union, aggregate functions */
(SELECT file_name, file_size FROM FILE) UNION (SELECT 'SUM' AS file_name, SUM(file_size) file_size FROM FILE) UNION (SELECT 'AVERAGE' AS file_name, AVG(file_size) file_size FROM FILE) UNION (SELECT 'MAX' AS file_name, MAX(file_size) file_size FROM FILE) UNION (SELECT 'MIN' AS file_name, MIN(file_size) file_size FROM FILE);

/* 6 (additional non-simplistic queries)
Purpose: to find popular / most interacted with files
Query: lists all the files with their matching date data if and only if they have been downloaded AND modified
       at least once. The result is ordered by creation time, and only the first modification/edit
       date is printed, no duplicate download or modification dates.
Uses: inner join */
SELECT file_name, creation_time, d_time, mod_date FROM FILE, DOWNLOAD, MODIFICATION WHERE (FILE.svg_id = DOWNLOAD.svg_id || FILE.svg_id = MODIFICATION.svg_id) GROUP BY file_name ORDER BY creation_time;

/*
Purpose: find most common downloader hostname and lists all the files they have downloaded
Query: displays the file names and download times of most common download host
Uses: nested queries, inner join
*/
SELECT file_name, d_time, d_hostname FROM (FILE INNER JOIN DOWNLOAD ON FILE.svg_id=DOWNLOAD.svg_id) WHERE d_hostname = (SELECT d_hostname FROM DOWNLOAD GROUP BY d_hostname ORDER BY COUNT(*) DESC LIMIT 1);
