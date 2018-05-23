#include<iostream>
#include<map>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

#define MAXINSTRUCTION 10
#define FILENAME "data.txt"
#define INSTRUCTION pair<OP, pair<REG, pair<REG, REG>>>
#define PIPOUT 0
#define READPPARAM 1
#define EXE 2
#define WRITEBACK 3
#define ADDCIRCLE 2
#define MULCIRCLE 10
#define DIVCIRCLE 40
#define SLCIRLCE 1


//���ܲ���
enum FU{FU_NIL, Integer, Mult1, Mult2, Add, Divide};
const int fu_size = 5;

//��������
enum OP{OP_NIL, L_D, S_D, SUB_D, ADD_D, DIV_D, MULT_D};

//�Ĵ�������
const int R_size = 31;
enum REG{R_NIL = R_size, R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14,
		R15, R16, R17, R18, R19, R20, R21, R22, R23, R24, R25, R26,
		R27, R28, R29, R30};
const char * const reg[] = {
	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	"R16", "R17", "R18", "R19", "R20", "R21", "R22", "R23", "R24", "R25", "R26", "R27", "R328", "R29", "R30" };

//ָ��״̬��
struct instructStatus{
	OP op;
	REG D;
	REG S1;
	REG S2;
	int imm;
	bool *status;
	instructStatus(){
		D = REG::R_NIL;
		S1 = REG::R_NIL;
		S2 = REG::R_NIL;
		status = new bool[4];// �ĸ�״̬�ֱ��ʾ������ ���������� ִ�У� д���
		memset(this, 0, sizeof(instructStatus));
	}
	instructStatus(OP _op, REG _D, REG _S1, REG _S2, int _imm){//ָ���ʽ��op r0, r1, r2
		status = new bool[4];// �ĸ�״̬�ֱ��ʾ������ ���������� ִ�У� д���
		memset(status, 0, sizeof(bool)*4);
		op = _op;
		D = _D;
		S1 = _S1;
		S2 = _S2;
		imm = _imm;
	}
	instructStatus(OP _op, REG _D, REG _S1, int _imm){//ָ���ʽ��op r0, r1
		status = new bool[4];// �ĸ�״̬�ֱ��ʾ������ ���������� ִ�У� д���
		memset(status, 0, sizeof(bool)* 4);
		op = _op;
		D = _D;
		S1 = _S1;
		imm = _imm;
	}
};

vector<string> instructions;

//���ܲ�����
struct funStatus{
	map<FU, bool> busy;
	map<FU, OP> op;
	map<FU, REG> Fi;
	map<FU, REG> Fj;
	map<FU, REG> Fk;
	map<FU, FU> Qj;
	map<FU, FU> Qk;
	map<FU, bool> Rj;
	map<FU, bool> Rk;
	//��ʼ������״̬��
	funStatus(){
		for (FU tempFu = Integer; tempFu<fu_size; tempFu = (FU)(tempFu + 1)){
			busy.insert(make_pair(tempFu, false));
			op.insert(make_pair(tempFu, OP::OP_NIL));
			Fi.insert(make_pair(tempFu, REG::R_NIL));
			Fj.insert(make_pair(tempFu, REG::R_NIL));
			Fk.insert(make_pair(tempFu, REG::R_NIL));
			Qj.insert(make_pair(tempFu, FU::FU_NIL));
			Qk.insert(make_pair(tempFu, FU::FU_NIL));
			Rj.insert(make_pair(tempFu, false));
			Rk.insert(make_pair(tempFu, false));
		}
	}
};

//����Ĵ���״̬��
struct resultRstatus{
	FU* resultStatus;
	resultRstatus(){
		resultStatus = new FU[R_size+1];
		for (int i = 0; i < R_size+1; i++){
			resultStatus[i] = FU::FU_NIL;
		}
	}
};

struct Score{
	int current;//��¼��ǰ����ָ��λ��
	int instr_size;
	//vector<instructStatus *>I_status;
	instructStatus **I_status;
	funStatus *F_status;
	resultRstatus *R_status;
	Score(){
		current = 0;
		I_status = new instructStatus*[MAXINSTRUCTION];
		F_status = new funStatus();
		R_status = new resultRstatus();
	}
};

int loadData(Score* S);

INSTRUCTION parse(char* s, int &imm);

FU getFU(instructStatus *Ipointer);

int getCirlce(instructStatus *Ipointer);

void display(Score *S);

//����ָ��
bool pipOut(Score *S);

//��������
bool readOpParam(Score *S);

//ִ��
bool execute(Score *S);

//д���
bool writeBack(Score *S);

int main(){
	Score* S = new Score();
	S->instr_size = loadData(S);
	while (!S->I_status[S->instr_size-1]->status[WRITEBACK]){
		display(S);
		pipOut(S);
		readOpParam(S);
		execute(S);
		writeBack(S);
	}
	system("pause");
}

map<instructStatus*, int> simExe;

int loadData(Score *S){
	int data_size = 0;
	fstream fout(FILENAME, std::ios::in);
	char* buffer = new char[20];
	stringstream ss;
	string temp, d_r;
	int count = 0, imm = -1;
	if (fout){
		fout.getline(buffer, 20);
		instructions.push_back((string)buffer);
		INSTRUCTION instr = parse(buffer, imm);
		S->I_status[count++] = new instructStatus(instr.first, instr.second.first, 
			instr.second.second.first, instr.second.second.second, imm);
		data_size++;
	}
	delete[] buffer;
	return data_size;
}

bool pipOut(Score *S){
	if (S->current == S->instr_size)
		return false;
	instructStatus *Ipointer = S->I_status[S->current];
	FU fun;
	bool isPip = false;
	getFU(Ipointer);
	//�豸�Ƿ���У����Ƿ����WAW��ͻ
	if (!S->F_status->busy[fun] && S->R_status->resultStatus[Ipointer->D]==FU::FU_NIL){
		isPip = true;
	}
	else if (fun == FU::Mult1){//�˷�����������������Ҫ�����ж���һ���豸�Ƿ����
		fun = FU::Mult2;
		if (!S->F_status->busy[fun] && S->R_status->resultStatus[Ipointer->D] == FU::FU_NIL){
			isPip = true;
		}
	}
	if (isPip){
		S->F_status->busy[fun] = true;//�������æ
		S->I_status[S->current]->status[PIPOUT] = true;//��¼����
		S->current++;
		S->F_status->op[fun] = Ipointer->op;//��¼������
		S->F_status->Fi[fun] = Ipointer->D;//��¼Ŀ�ļĴ������
		S->F_status->Fj[fun] = Ipointer->S1;//��¼Դ�Ĵ������
		S->F_status->Fk[fun] = Ipointer->S2;//��¼Դ�Ĵ������
		S->F_status->Qj[fun] = S->R_status->resultStatus[Ipointer->S1];//���Դ��������Դ����
		S->F_status->Qk[fun] = S->R_status->resultStatus[Ipointer->S2];//���Դ��������Դ����
		S->F_status->Rj[fun] = (S->F_status->Qj[fun] == FU::FU_NIL) ? false : true;//Դ�������Ƿ����
		S->F_status->Rk[fun] = (S->F_status->Qk[fun] == FU::FU_NIL) ? false : true;//Դ�������Ƿ����
	}
	return isPip;
}

bool readOpParam(Score *S){
	FU fun;
	bool read = false;
	for (int i = S->current - 1; i >= 0; i--){
		fun = getFU(S->I_status[i]);
		if ((S->F_status->Rj[fun] == true || S->F_status->Rj[fun] == FU::FU_NIL)&&
			(S->F_status->Rk[fun] == true || S->F_status->Rk[fun] == FU::FU_NIL)){//�������������������������Ѿ�׼������
			S->F_status->Rj[fun] = false;
			S->F_status->Rk[fun] = false;
			S->F_status->Qj[fun] = FU::FU_NIL;
			S->F_status->Qk[fun] = FU::FU_NIL;
			S->I_status[i]->status[READPPARAM] = true;
			read = true;
		}
	}
	return read;
}

bool execute(Score *S){
	FU fun;
	bool exe = false;
	for (int i = S->current - 1; i >= 0; i--){
		instructStatus *instr = S->I_status[i];
		if (instr->status[READPPARAM] && !instr->status[EXE]){
			//�˴�������չ��Ӿ������
			if (simExe.count(instr) == 0){
				simExe.insert(make_pair(instr, getCirlce(instr)));
			}
			simExe[instr]--;
			if (simExe[instr] == 0){
				instr->status[EXE] = true;
				simExe.erase(instr);
				exe = true;
			}
		}
	}
	return exe;
}

bool writeBack(Score *S){
	FU fun;
	bool exe = false, flag;
	for (int i = 0; i < S->current; i++){//��������ָ��
		instructStatus *instr = S->I_status[i];
		if (instr->status[EXE]){//ֻ��ִ����ɵ�ָ����к�������
			flag = true;
			for (FU j = FU::Integer; j < fu_size; j = (FU)(j+1)){//�������е�Ԫ�������м��
				if (!((S->F_status->Fj[j] != S->F_status->Fi[fun] || !S->F_status->Rj[j]) &&
					(S->F_status->Fk[j] != S->F_status->Fi[fun] || !S->F_status->Rk[j]))){//���û��WAR��ͻ��д��
					flag = false;
					break;
				}
			}
			if (flag){
				//�˴��������д�ز���
				//...................
				exe = true;
				S->I_status[i]->status[WRITEBACK] = true;
				for (FU k = FU::Integer; k < fu_size; k = (FU)(k + 1)){
					if (S->F_status->Qj[k] == i){//�����ָ���ڵȴ������������ͷ�
						S->F_status->Rj[k] = true;
					}
					if (S->F_status->Qk[k] == i){
						S->F_status->Rk[k] = true;
					}
				}
			}
		}
	}
	return exe;
}

INSTRUCTION parse(char* s, int &imm){
	string temp = s;
	while (temp.find(",") != -1){
		temp.replace(temp.find(","),1, "");
	}
	stringstream ss;
	ss << temp;
	string s_op, s_d, s_s1, s_s2, s_imm;
	ss >> s_op;
	ss >> s_d;
	ss >> s_s1;
	ss >> s_s2;
	OP op;
	REG d, s1, s2;
	bool isMem = false;
	if (s_op.find("L.D")!=-1){
		op = OP::L_D;
		isMem = true;
	}
	else if (s_op.find("S.D")!=-1){
		op = OP::S_D;
		isMem = true;
	}
	else if (s_op.find("MULT.D")!=-1){
		op = OP::MULT_D;
	}
	else if (s_op.find("SUB.D")!=-1){
		op = OP::SUB_D;
	}
	else if (s_op.find("ADD.D")!=-1){
		op = OP::ADD_D;
	}
	else if (s_op.find("DIV.D")!=-1){
		op = OP::DIV_D;
	}
	else{
		op = OP::OP_NIL;
	}
	//��ȡĿ�ļĴ���
	s_d = s_d.substr(1);
	d = (REG)atoi(s_d.c_str());
	//��ȡԴ�Ĵ���
	if (isMem){
		s_imm = s_s1.substr(0, s_s1.find("("));
		imm = atoi(s_imm.c_str());
		s_s1 = s_s1.substr(s_s1.find("R") + 1, s_s1.length() - s_s1.find("R")-2);
		cout << s_s1;
		s1 = (REG)atoi(s_s1.c_str());
		s2 = REG::R_NIL;
	}
	else{
		s_s1 = s_s1.substr(1);
		s1 = (REG)atoi(s_s1.c_str());
		s_s2 = s_s2.substr(1);
		s2 = (REG)atoi(s_s2.c_str());
	}
	return make_pair(op, make_pair(d, make_pair(s1, s2)));
}

FU getFU(instructStatus *Ipointer){
	FU fun;
	if (Ipointer->op == OP::ADD_D || Ipointer->op == OP::SUB_D){
		fun = FU::Add;
	}
	else if (Ipointer->op == OP::L_D || Ipointer->op == OP::S_D){
		fun = FU::Integer;
	}
	else if (Ipointer->op == OP::MULT_D){
		fun = FU::Mult1;
	}
	else{
		fun = FU::Divide;
	}
	return fun;
}

int getCirlce(instructStatus *Ipointer){
	FU fun = getFU(Ipointer);
	int circle = -1;
	if (FU::Add == fun)
		circle = ADDCIRCLE;
	else if (FU::Divide == fun)
		circle = DIVCIRCLE;
	else if (FU::Mult1 == fun)
		circle = MULCIRCLE;
	else if (FU::Integer == fun)
		circle = SLCIRLCE;
	return circle;
}

void display(Score *S){
	cout << "ָ��״̬��" << endl;
	cout << "ָ�� \t ���� \t ��������\t ִ��\t д���\t" << endl;
	for (int i = 0; i < S->instr_size; i++){
		cout << instructions[i] << "\t" << S->I_status[i]->status[PIPOUT] << "\t"
			<< S->I_status[i]->status[READPPARAM] << "\t" << S->I_status[i]->status[EXE]
			<< "\t" << S->I_status[i]->status[WRITEBACK] << endl;
	}
	//cout << "-------------------------------------------------------------------------" << endl;
	//cout << "���ܲ�����"
}