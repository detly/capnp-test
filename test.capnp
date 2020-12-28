@0xa03ece5b64831776;

struct Thing {
    union {
        first  @0 :EnumOne;
        second @1 :UnionTwo;
        third  @2 :UInt16;
    }
}

enum EnumOne {
    oneA @0;
}

struct UnionTwo {
    union {
        twoA @0 :UInt16;
        twoB @1 :UInt16;
    }
}
