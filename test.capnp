@0xa03ece5b64831776;

struct Thing {
    first @0 :UInt16;
    second @1 :EnumOne;
    third @2 :UInt16;
    fourth @3 :UInt16;
    fifth @4 :EnumTwo;
}

enum EnumOne {
    oneA @0;
    oneB @1;
    oneC @2;
}

enum EnumTwo {
    twoA @0;
    twoB @1;
    twoC @2;
}
