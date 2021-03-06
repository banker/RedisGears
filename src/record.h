/*
 * record.h
 *
 *  Created on: Oct 21, 2018
 *      Author: meir
 */

#ifndef SRC_RECORD_H_
#define SRC_RECORD_H_

#include "redisgears.h"
#include "utils/buffer.h"
#include "execution_plan.h"
#ifdef WITHPYTHON
#include <Python.h>
#endif

typedef struct RecordType{
    size_t id;
    char* name;
    size_t size;
    int (*sendReply)(Record* record, RedisModuleCtx* rctx);
    int (*serialize)(ExecutionCtx* ectx, Gears_BufferWriter* bw, Record* base);
    Record* (*deserialize)(ExecutionCtx* ectx, Gears_BufferReader* br);
    void (*free)(Record* base);
}RecordType;

typedef struct KeysHandlerRecord{
    Record base;
    RedisModuleKey *keyHandler;
}KeysHandlerRecord;

typedef struct LongRecord{
    Record base;
    long num;
}LongRecord;

typedef struct DoubleRecord{
    Record base;
    double num;
}DoubleRecord;

typedef struct StringRecord{
    Record base;
    size_t len;
    char* str;
}StringRecord;

typedef struct ListRecord{
    Record base;
    Record** records;
}ListRecord;

typedef struct KeyRecord{
    Record base;
    char* key;
    size_t len;
    Record* record;
}KeyRecord;

typedef struct HashSetRecord{
    Record base;
    Gears_dict* d;
}HashSetRecord;

typedef struct AsyncRecord{
    Record base;
    StepPendingCtx* pctx;
    Record** rptx; // pointer to put the record once ready
    Record** overridePlaceHolder; // pointer to put the actual record for accumulate, in this case we will put DummyRecord to rptx
    Record* originRecord;
}AsyncRecord;

extern RecordType* listRecordType;
extern RecordType* stringRecordType;
extern RecordType* errorRecordType;
extern RecordType* longRecordType;
extern RecordType* doubleRecordType;
extern RecordType* keyRecordType;
extern RecordType* keysHandlerRecordType;
extern RecordType* hashSetRecordType;
extern RecordType* asyncRecordType;
extern Record StopRecord;
extern Record WaitRecord;
extern Record DummyRecord;

#define IS_SPECIAL_RECORD(r) (r == &StopRecord || r == &WaitRecord || r == &DummyRecord)

void RG_FreeRecord(Record* record);
RecordType* RG_RecordGetType(Record* r);

/** key record api **/
Record* RG_KeyRecordCreate();
void RG_KeyRecordSetKey(Record* r, char* key, size_t len);
void RG_KeyRecordSetVal(Record* r, Record* val);
Record* RG_KeyRecordGetVal(Record* r);
char* RG_KeyRecordGetKey(Record* r, size_t* len);

/** list record api **/
Record* RG_ListRecordCreate(size_t initial_size);
size_t RG_ListRecordLen(Record* r);
void RG_ListRecordAdd(Record* r, Record* element);
Record* RG_ListRecordGet(Record* r, size_t index);
Record* RG_ListRecordPop(Record* r);

/** string record api **/
Record* RG_StringRecordCreate(char* val, size_t len);
char* RG_StringRecordGet(Record* r, size_t* len);
void RG_StringRecordSet(Record* r, char* val, size_t len);

/** double record api **/
Record* RG_DoubleRecordCreate(double val);
double RG_DoubleRecordGet(Record* r);
void RG_DoubleRecordSet(Record* r, double val);

/** long record api **/
Record* RG_LongRecordCreate(long val);
long RG_LongRecordGet(Record* r);
void RG_LongRecordSet(Record* r, long val);

/** hash set record api **/
Record* RG_HashSetRecordCreate();
int RG_HashSetRecordSet(Record* r, char* key, Record* val);
Record* RG_HashSetRecordGet(Record* r, char* key);
char** RG_HashSetRecordGetAllKeys(Record* r);
void RG_HashSetRecordFreeKeysArray(char** keyArr);

/** async record api **/
Record* RG_AsyncRecordCreate(ExecutionCtx* ectx, char** err);
void RG_AsyncRecordContinue(Record* asyncRecord, Record* r);

/* todo: think if we can removed this!! */
Record* RG_KeyHandlerRecordCreate(RedisModuleKey* handler);
RedisModuleKey* RG_KeyHandlerRecordGet(Record* r);

int RG_SerializeRecord(ExecutionCtx* ctx, Gears_BufferWriter* bw, Record* r);
Record* RG_DeserializeRecord(ExecutionCtx* ctx, Gears_BufferReader* br);
int RG_RecordSendReply(Record* record, RedisModuleCtx* rctx);

Record* RG_ErrorRecordCreate(char* val, size_t len);

void Record_Initialize();
Record* RG_RecordCreate(RecordType* type);
RecordType* RG_RecordTypeCreate(const char* name, size_t size,
                                RecordSendReply,
                                RecordSerialize,
                                RecordDeserialize,
                                RecordFree);




#endif /* SRC_RECORD_H_ */
