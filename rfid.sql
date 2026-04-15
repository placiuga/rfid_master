/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19-11.8.3-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: rfid
-- ------------------------------------------------------
-- Server version	11.8.3-MariaDB-0+deb13u1 from Debian

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*M!100616 SET @OLD_NOTE_VERBOSITY=@@NOTE_VERBOSITY, NOTE_VERBOSITY=0 */;

--
-- Table structure for table `LoggingTable`
--

DROP TABLE IF EXISTS `LoggingTable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `LoggingTable` (
  `UsageID` int(11) NOT NULL AUTO_INCREMENT,
  `startTime` datetime DEFAULT NULL,
  `endTime` datetime DEFAULT NULL,
  `MachineID` int(11) DEFAULT NULL,
  `StudentID` varchar(20) DEFAULT NULL,
  `Notes` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`UsageID`),
  KEY `MachineID` (`MachineID`),
  KEY `StudentID` (`StudentID`),
  CONSTRAINT `LoggingTable_ibfk_1` FOREIGN KEY (`MachineID`) REFERENCES `MachineTable` (`MachineID`),
  CONSTRAINT `LoggingTable_ibfk_2` FOREIGN KEY (`StudentID`) REFERENCES `UserTable` (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `LoggingTable`
--

LOCK TABLES `LoggingTable` WRITE;
/*!40000 ALTER TABLE `LoggingTable` DISABLE KEYS */;
set autocommit=0;
/*!40000 ALTER TABLE `LoggingTable` ENABLE KEYS */;
UNLOCK TABLES;
commit;

--
-- Table structure for table `MachineAuthorizations`
--

DROP TABLE IF EXISTS `MachineAuthorizations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `MachineAuthorizations` (
  `AuthorizationID` int(11) NOT NULL,
  `MachineID` int(11) DEFAULT NULL,
  `granted_by` varchar(100) DEFAULT NULL,
  `notes` varchar(255) DEFAULT NULL,
  `UserID` int(11) NOT NULL,
  PRIMARY KEY (`AuthorizationID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `MachineAuthorizations`
--

LOCK TABLES `MachineAuthorizations` WRITE;
/*!40000 ALTER TABLE `MachineAuthorizations` DISABLE KEYS */;
set autocommit=0;
/*!40000 ALTER TABLE `MachineAuthorizations` ENABLE KEYS */;
UNLOCK TABLES;
commit;

--
-- Table structure for table `MachineTable`
--

DROP TABLE IF EXISTS `MachineTable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `MachineTable` (
  `MachineID` int(11) NOT NULL,
  `MachineName` varchar(100) NOT NULL,
  `ArduinoMAC` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`MachineID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `MachineTable`
--

LOCK TABLES `MachineTable` WRITE;
/*!40000 ALTER TABLE `MachineTable` DISABLE KEYS */;
set autocommit=0;
INSERT INTO `MachineTable` VALUES
(0,'Machine Does Not Exist',NULL);
/*!40000 ALTER TABLE `MachineTable` ENABLE KEYS */;
UNLOCK TABLES;
commit;

--
-- Table structure for table `UserTable`
--

DROP TABLE IF EXISTS `UserTable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `UserTable` (
  `UserID` varchar(20) NOT NULL,
  `Name` varchar(100) NOT NULL,
  `FSUID` varchar(20) DEFAULT NULL,
  `Status` varchar(20) DEFAULT NULL,
  `rfid_uid` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`UserID`),
  UNIQUE KEY `rfid_uid` (`rfid_uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `UserTable`
--

LOCK TABLES `UserTable` WRITE;
/*!40000 ALTER TABLE `UserTable` DISABLE KEYS */;
set autocommit=0;
INSERT INTO `UserTable` VALUES
('DNE','Student Does Not Exist',NULL,NULL,NULL);
/*!40000 ALTER TABLE `UserTable` ENABLE KEYS */;
UNLOCK TABLES;
commit;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*M!100616 SET NOTE_VERBOSITY=@OLD_NOTE_VERBOSITY */;

-- Dump completed on 2026-03-25  3:42:55
