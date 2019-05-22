DROP TABLE TB_CLOB;
CREATE TABLE TB_CLOB
(
C_LOB CLOB 
,ID INT NOT NULL
,DATABASENAME   VARCHAR(255)
,INSTANCENAME  VARCHAR(255)
,USERNAME      VARCHAR(255)
,CREATETIME    TIMESTAMP
,FDBVERSIONMAJOR    INT
,FDBVERSIONMINOR    INT
,FDBVERSIONBUGFIX   INT
,FDEPROVIDERNAME    VARCHAR(255)
,FDEDESCRIPTION     VARCHAR(104)
,UID                VARCHAR(64)
);