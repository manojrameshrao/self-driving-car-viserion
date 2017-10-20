-----------------------------------------------------------------------------
# 								Viserion

This is a repository for SJSU CMPE 243 - Viserion project group.
The Master branch has been cloned from the Professor Preetpal Kang's 
https://gitlab.com/preet/lpc1758_freertos git repository

Wiki:
http://www.socialledge.com/sjsu/index.php?title=F17:_Viserion

**Always use git bash to push/pull from the main directory (Do not use Eclipse)**

- Viserion.dbc will be the dbc file that will be shared by all 5 projects.
- dbc_parse.py is also decoupled from each internal directory so that it will be easier to change if required in future.
- prebuild.sh of every project has been modified to find the python script and dbc file in the main directory.
- /_can_dbc path has been added to search paths of every project so that there are no errors when the autogenerated header file is included.


To clone the project use:
```sh
$ git clone https://gitlab.com/aakashmenon/Viserion.git
```

Git Development Workflow:
1. Create a new branch.

```	$ git checkout -b <branchname> 
```
2. Develop code as required
and stage files
To stage all files

```	$ git add -A
```
OR
To stage particular files

```	$ git add <filename1> <filename2>
```
3. Commit the changes to the local repository.

```	$ git commit -m "add comments"
```
4. Push the branch to remote repository.

```	$ git push origin head
```
5. **Switch to master and update it to the latest version.**

```	$ git checkout master
	$ git pull origin master
```
6. Now merge and push the branch.

```	$ git merge <branchname>
	$ git push origin master
```

-----------------------------------------------------------------------------