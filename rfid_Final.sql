/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19-11.8.6-MariaDB, for debian-linux-gnu (aarch64)
--
-- Host: localhost    Database: rfid
-- ------------------------------------------------------
-- Server version	11.8.6-MariaDB-0+deb13u1 from Debian

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
  `UserID` varchar(20) DEFAULT NULL,
  `Notes` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`UsageID`),
  KEY `MachineID` (`MachineID`),
  KEY `StudentID` (`UserID`),
  CONSTRAINT `LoggingTable_ibfk_1` FOREIGN KEY (`MachineID`) REFERENCES `MachineTable` (`MachineID`),
  CONSTRAINT `LoggingTable_ibfk_2` FOREIGN KEY (`UserID`) REFERENCES `UserTable` (`UserID`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `LoggingTable`
--

SET @OLD_AUTOCOMMIT=@@AUTOCOMMIT, @@AUTOCOMMIT=0;
LOCK TABLES `LoggingTable` WRITE;
/*!40000 ALTER TABLE `LoggingTable` DISABLE KEYS */;
INSERT INTO `LoggingTable` VALUES
(1,'2026-04-15 17:50:39','2026-04-15 17:50:39',1,'DNE','Failed RFID: 2BC3B1FB'),
(2,'2026-04-15 17:50:49','2026-04-15 17:50:49',1,'DNE','Failed RFID: 229237AE'),
(3,'2026-04-15 17:50:57','2026-04-15 17:50:57',1,'DNE','Failed RFID: 523625AE'),
(4,'2026-04-15 17:54:11','2026-04-15 17:54:11',1,'1','Authorization Failed'),
(5,'2026-04-15 17:54:51','2026-04-15 17:54:51',1,'1','Authorization Failed'),
(10,'2026-04-15 18:24:45','2026-04-15 18:24:47',2,'2','Successful Authorization'),
(11,'2026-04-15 18:25:00','2026-04-15 18:25:00',2,'2','Authorization Failed'),
(12,'2026-04-15 18:25:20','2026-04-15 18:25:21',2,'2','Successful Authorization'),
(13,'2026-04-15 18:30:24','2026-04-15 18:30:25',2,'2','Successful Authorization'),
(14,'2026-04-15 18:32:06','2026-04-15 18:32:07',2,'2','Successful Authorization'),
(15,'2026-04-15 18:32:08','2026-04-15 18:32:10',2,'2','Successful Authorization'),
(16,'2026-04-15 18:33:22','2026-04-15 18:33:23',2,'1','Successful Authorization'),
(17,'2026-04-15 18:33:24','2026-04-15 18:33:25',2,'2','Successful Authorization'),
(18,'2026-04-15 18:33:27','2026-04-15 18:33:27',2,'DNE','Failed RFID: 523625AE'),
(19,'2026-04-15 18:33:29','2026-04-15 18:33:29',2,'DNE','Failed RFID: 523625AE'),
(20,'2026-04-15 18:33:31','2026-04-15 18:33:32',2,'1','Successful Authorization'),
(21,'2026-04-15 18:33:32','2026-04-15 18:33:33',2,'1','Successful Authorization'),
(22,'2026-04-15 18:33:34','2026-04-15 18:33:37',2,'1','Successful Authorization'),
(23,'2026-04-15 19:26:02','2026-04-15 19:26:03',2,'2','Successful Authorization');
/*!40000 ALTER TABLE `LoggingTable` ENABLE KEYS */;
UNLOCK TABLES;
COMMIT;
SET AUTOCOMMIT=@OLD_AUTOCOMMIT;

--
-- Table structure for table `MachineAuthorizations`
--

DROP TABLE IF EXISTS `MachineAuthorizations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `MachineAuthorizations` (
  `AuthorizationID` int(11) NOT NULL AUTO_INCREMENT,
  `MachineID` int(11) DEFAULT NULL,
  `granted_by` varchar(100) DEFAULT NULL,
  `notes` varchar(255) DEFAULT NULL,
  `UserID` varchar(20) NOT NULL,
  PRIMARY KEY (`AuthorizationID`),
  KEY `fk_ma_user` (`UserID`),
  KEY `fk_ma_machine` (`MachineID`),
  CONSTRAINT `fk_ma_machine` FOREIGN KEY (`MachineID`) REFERENCES `MachineTable` (`MachineID`),
  CONSTRAINT `fk_ma_user` FOREIGN KEY (`UserID`) REFERENCES `UserTable` (`UserID`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `MachineAuthorizations`
--

SET @OLD_AUTOCOMMIT=@@AUTOCOMMIT, @@AUTOCOMMIT=0;
LOCK TABLES `MachineAuthorizations` WRITE;
/*!40000 ALTER TABLE `MachineAuthorizations` DISABLE KEYS */;
INSERT INTO `MachineAuthorizations` VALUES
(1,2,'calvin','','1'),
(4,2,'','','2');
/*!40000 ALTER TABLE `MachineAuthorizations` ENABLE KEYS */;
UNLOCK TABLES;
COMMIT;
SET AUTOCOMMIT=@OLD_AUTOCOMMIT;

--
-- Table structure for table `MachineTable`
--

DROP TABLE IF EXISTS `MachineTable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `MachineTable` (
  `MachineID` int(11) NOT NULL AUTO_INCREMENT,
  `MachineName` varchar(100) DEFAULT NULL,
  `ArduinoMAC` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`MachineID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `MachineTable`
--

SET @OLD_AUTOCOMMIT=@@AUTOCOMMIT, @@AUTOCOMMIT=0;
LOCK TABLES `MachineTable` WRITE;
/*!40000 ALTER TABLE `MachineTable` DISABLE KEYS */;
INSERT INTO `MachineTable` VALUES
(1,'Machine Does Not Exist',NULL),
(2,NULL,'B42A7A842178');
/*!40000 ALTER TABLE `MachineTable` ENABLE KEYS */;
UNLOCK TABLES;
COMMIT;
SET AUTOCOMMIT=@OLD_AUTOCOMMIT;

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

SET @OLD_AUTOCOMMIT=@@AUTOCOMMIT, @@AUTOCOMMIT=0;
LOCK TABLES `UserTable` WRITE;
/*!40000 ALTER TABLE `UserTable` DISABLE KEYS */;
INSERT INTO `UserTable` VALUES
('1','test','ttt22e','active','2BC3B1FB'),
('2','guy','ccccc33','active','229237AE'),
('DNE','Student Does Not Exist',NULL,NULL,NULL);
/*!40000 ALTER TABLE `UserTable` ENABLE KEYS */;
UNLOCK TABLES;
COMMIT;
SET AUTOCOMMIT=@OLD_AUTOCOMMIT;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*M!100616 SET NOTE_VERBOSITY=@OLD_NOTE_VERBOSITY */;

-- Dump completed on 2026-04-16 20:08:38
