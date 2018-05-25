void decodeMult(STATE *state);

void decodeProcess(STATE *state);

void decodeTransfer(STATE *state);

void decodeBranch(STATE *state);

void executeProcess(STATE *state);

void executeMult(STATE *state);

void executeTransfer(STATE *state);

void executeBranch(STATE *state);

word processOp2(STATE *state);

void print(STATE *ptr);

//given a word, checks for cond (1 = go, 0 = no)
int checkCond(word instruction, word cpsr) ;

void bitAnd(STATE *state, bool b);

void bitEor(STATE *state, bool b);

void subRO(STATE *state, bool b);

void subOR(STATE *state);

void addRO(STATE *state);

void bitOR(STATE *state);

void processMove(STATE *state);