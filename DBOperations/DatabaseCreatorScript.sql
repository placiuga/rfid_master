CREATE TABLE MachineTable (
	MachineID INT NOT NULL,
    MachineName VARCHAR(100) NOT NULL,
    PRIMARY KEY (MachineID));
    
CREATE TABLE StudentTable (
	StudentID VARCHAR(20) NOT NULL,
	StudentName VARCHAR(100) NOT NULL,
    PRIMARY KEY(StudentID));
    
CREATE TABLE LoggingTable (
	UsageID INT NOT NULL AUTO_INCREMENT,
    startTime datetime DEFAULT NULL,
    endTime datetime DEFAULT NULL,
    MachineID INT DEFAULT NULL,
    StudentID VARCHAR(20) DEFAULT NULL,
	Notes VARCHAR(100) DEFAULT NULL,
    PRIMARY KEY (UsageID),
    FOREIGN KEY (MachineID) REFERENCES MachineTable (MachineID),
    FOREIGN KEY (StudentID) REFERENCES StudentTable (StudentID));
    
CREATE TABLE AuthenticationTable (
	MachineID INT NOT NULL,
    StudentID VARCHAR(20) NOT NULL,
    PRIMARY KEY (MachineID, StudentID),
    FOREIGN KEY (MachineID) REFERENCES MachineTable (MachineID),
    FOREIGN KEY (StudentID) REFERENCES StudentTable (StudentID));
    
INSERT INTO MachineTable (MachineID, MachineName)
	VALUES(0, 'Machine Does Not Exist');

INSERT INTO StudentTable (StudentID, StudentName)
	VALUES('DNE', 'Student Does Not Exist');