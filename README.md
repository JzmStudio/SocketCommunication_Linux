# SocketCommunication_Linux
Communication on web and used on Linux
git on Ubuntu

## To deploy server
---
First:
You need these packages:mysql-server mysql-client libmysqlclient-dev
And you have to deploy your mysql.Use a database and create a table named 'users'.Then create a procedure with the code below:
delimiter /
CREATE PROCEDURE `login`(IN idin int unsigned,IN pw char(16),OUT n char(16),OUT bool char(1))
	begin
	select name into n from users where id=idin and password=pw;
	if n is null then set bool='n';
	else set bool='y';
	end if;
	end

Second:
Push the code to a directory.
Open mysql.conf and replace 'Mysql_root_password' and 'Your_database_name' to your password and database name.
Open the path on terminal.Then input:
gcc makerserver -o makeserver
./makeserver
./server
Then the server opened.
---
## To deploy client
---
Push the code to a directory.And Open the path on terminal.Input:
gcc makeclient -o makeclient
./makeclient
./client
Then the client opened.
