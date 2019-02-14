DROP TABLE TB_TEST_1;
CREATE TABLE TB_TEST_1
(
TRANS_TRACK_ID                     BLOB
, NODE_INS_ID                      VARCHAR(40 ) 
, ACTIVE_INS_ID                    VARCHAR(40 ) 
, NODE_TRACK_ID                    VARCHAR(40) 
, WORK_ID                          VARCHAR(40 ) 
, TRANS_FLAG                       NUMERIC(4, 0) 
, ACTOR_ID                         VARCHAR(64 ) 
, ACTOR_NAME                       VARCHAR(50 )
, AUTHORIZER_ID                    VARCHAR(64 ) 
, AUTHORIZER_NAME                  VARCHAR(64 ) 
, CREATE_TIME                      TIMESTAMP
, HANDLE_TIME                      TIMESTAMP
, OVER_TIME                        TIMESTAMP
, MSG                              VARCHAR(40 )
, MSG_TYPE                         NUMERIC(4, 0) 
, RESULT_FLAG                      VARCHAR(20 ) 
, BACK_FLAG                        NUMERIC(4, 0) 
, CONDITION_1                        VARCHAR(100 ) 
, CONDITION_VALUE                  VARCHAR(100 ) 
, CONDITION_RESULT                 NUMERIC(4, 0)
, RESSIGNM_PERSON_ID               VARCHAR(64 ) 
, RESSIGNM_PERSON_NAME             VARCHAR(64 ) 
, COMPLETE_TYPE                    NUMERIC(4, 0)
, REVOKE_FLAG                      NUMERIC(3, 0) 
, PRE_TRANS_TRACK_ID               VARCHAR(40 ) 
, PROCESS_ID                       VARCHAR(40 ) 
, MAIN_PROCESS_INS_ID              VARCHAR(40 ) 
, MODIFY_DATE                      TIMESTAMP
);
