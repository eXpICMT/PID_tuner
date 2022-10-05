# How to use it. 

## 1. Convert .db to .csv by using "Export CSV" in SQLite. For example, use dups_sql09_22_093022_SoC.db.
## 2. Copy and paste target measurements column to .txt , e.g. "Ubat" in my case. The target range is {111...4075} in .csv or {110...4074} .db.
## 3. Rename "*.txt" to "test.txt".
## 4. This app will create the new .db, including the column of target measurements from test.txt and columns of estimated values.
## 5. By estimating real values, the app is using:
## - alpha-beta filter;
## - median filter;
## - tracking of a negative differential between previous and actual values of target measurements;

### If you wish, you can look at example results in db_pid_tuner_sql10_22.db.

## 6. in process...
