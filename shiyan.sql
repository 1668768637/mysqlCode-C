/*
Navicat MySQL Data Transfer

Source Server         : mysql
Source Server Version : 80027
Source Host           : localhost:3306
Source Database       : shiyan

Target Server Type    : MYSQL
Target Server Version : 80027
File Encoding         : 65001

Date: 2021-12-25 21:33:17
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `bookinginformation`
-- ----------------------------
DROP TABLE IF EXISTS `bookinginformation`;
CREATE TABLE `bookinginformation` (
  `name` char(9) NOT NULL,
  `id` char(20) DEFAULT NULL,
  `banci` char(20) DEFAULT NULL,
  `seatNumber` int DEFAULT NULL,
  `orderNumber` int DEFAULT NULL,
  PRIMARY KEY (`name`),
  KEY `1` (`banci`),
  KEY `orderNumber` (`orderNumber`),
  KEY `seatNumber` (`seatNumber`),
  CONSTRAINT `1` FOREIGN KEY (`banci`) REFERENCES `flightinformation` (`banci`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of bookinginformation
-- ----------------------------
INSERT INTO `bookinginformation` VALUES ('爱心', '2019010202', '2', '14', '214');

-- ----------------------------
-- Table structure for `flightinformation`
-- ----------------------------
DROP TABLE IF EXISTS `flightinformation`;
CREATE TABLE `flightinformation` (
  `banci` char(20) NOT NULL,
  `price` float(11,0) DEFAULT NULL,
  `startCity` char(9) DEFAULT NULL,
  `goalCity` char(9) DEFAULT NULL,
  `flyingTime` datetime DEFAULT NULL,
  `shippingSpaceResidue` int DEFAULT NULL,
  `shippingSpaceSum` int DEFAULT NULL,
  PRIMARY KEY (`banci`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of flightinformation
-- ----------------------------
INSERT INTO `flightinformation` VALUES ('1', '100', '新乡', '信阳', '2021-12-14 06:00:00', '15', '15');
INSERT INTO `flightinformation` VALUES ('2', '100', '新乡', '信阳', '2021-12-14 09:00:00', '14', '15');
INSERT INTO `flightinformation` VALUES ('3', '100', '新乡', '信阳', '2021-12-14 14:00:00', '15', '15');
INSERT INTO `flightinformation` VALUES ('4', '100', '新乡', '信阳', '2021-12-14 16:00:00', '15', '15');

-- ----------------------------
-- Table structure for `passenger`
-- ----------------------------
DROP TABLE IF EXISTS `passenger`;
CREATE TABLE `passenger` (
  `name` char(9) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `id` char(11) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `sex` char(3) DEFAULT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of passenger
-- ----------------------------
INSERT INTO `passenger` VALUES ('爱心', '2019010202', '男');
DROP TRIGGER IF EXISTS `insert_setnumber`;
DELIMITER ;;
CREATE TRIGGER `insert_setnumber` AFTER INSERT ON `bookinginformation` FOR EACH ROW BEGIN 
  UPDATE flightinformation SET shippingspaceresidue=shippingspaceresidue-1  WHERE flightinformation.banci=new.banci;
END
;;
DELIMITER ;
DROP TRIGGER IF EXISTS `DELETE_shippingspace`;
DELIMITER ;;
CREATE TRIGGER `DELETE_shippingspace` AFTER DELETE ON `bookinginformation` FOR EACH ROW BEGIN 
  UPDATE flightinformation SET shippingspaceresidue=shippingspaceresidue+1 WHERE banci=old.banci;
end
;;
DELIMITER ;
DROP TRIGGER IF EXISTS `insert_sex`;
DELIMITER ;;
CREATE TRIGGER `insert_sex` BEFORE INSERT ON `passenger` FOR EACH ROW BEGIN

DECLARE msg varchar(100);
IF new.sex!='鐢? AND  new.sex!='鐢?THEN
SET msg=CONCAT('鎮ㄨ緭鍏ョ殑鎬у埆锛?,new.sex,' 涓嶇鍚堣姹傦紝璇疯緭鍏ョ敺鎴栧コ銆?);
SIGNAL SQLSTATE 'HYOOO' SET message_text=msg;
END IF;
END
;;
DELIMITER ;
