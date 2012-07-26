# --------------------------------------------------------
# Host:                         127.0.0.1
# Server version:               5.5.8
# Server OS:                    Win64
# HeidiSQL version:             6.0.0.3603
# Date/time:                    2011-05-14 16:28:59
# --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

# Dumping database structure for world
CREATE DATABASE IF NOT EXISTS `world` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `world`;


# Dumping structure for table world.pvp_itemsets
CREATE TABLE IF NOT EXISTS `pvp_itemsets` (
  `itemid` int(10) DEFAULT NULL,
  `spec` tinyint(3) DEFAULT NULL,
  `rank` tinyint(3) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

# Dumping data for table world.pvp_itemsets: ~0 rows (approximately)
/*!40000 ALTER TABLE `pvp_itemsets` DISABLE KEYS */;
/*!40000 ALTER TABLE `pvp_itemsets` ENABLE KEYS */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
