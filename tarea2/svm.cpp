#include <iostream>

#include "svm.hh"

string snames[18] = { "push", "pop", "dup", "swap", "add", "sub", "mult", "div", "goto", "jmpeq", "jmpgt", "jmpge", "jmplt", "jmple", "skip", "store", "load", "print" };

Instruction::Instruction(string l, IType itype):label(l),type(itype),hasarg(false) {
}

Instruction::Instruction(string l, IType itype, int arg):label(l),type(itype),hasarg(true), argint(arg),jmplabel("") {
}

Instruction::Instruction(string l, IType itype, string arg):label(l),type(itype),hasarg(true),jmplabel(arg) {

}

SVM::SVM(list<Instruction*>& sl) {
  instructions.reserve(sl.size());
  copy(begin(sl), end(sl), back_inserter(instructions));
  pc = 0;
  // resolver labels
  for(int i=0; i < instructions.size(); i++) {
    string l = instructions[i]->label;
    if (l != "")
      labels[l] = i; 
  }
  for(int i=0; i < instructions.size(); i++) {
    string jl = instructions[i]->jmplabel;
    if (jl != "") {
      std::unordered_map<std::string,int>::const_iterator it = labels.find(jl);
      if (it == labels.end()) {
	cout << "No se encontro label " << jl << endl;
	exit(0);
      } else
	instructions[i]->argint = it->second;
    }
  }
  

					   
}

void SVM::execute() {
  while (true) {   
    if (pc >= instructions.size()) break;
    // cout << "pc " << pc << " ";
    // print_stack();
    execute(instructions[pc]);
  }
}

void SVM::execute(Instruction* instr) {
  Instruction::IType itype = instr->type;
  int next, top;
  //cout << "type: " << itype << endl;
  if (itype==Instruction::IPOP || itype==Instruction::IDUP || itype==Instruction::IPRINT || itype==Instruction::ISKIP) {
    switch (itype) {
    case(Instruction::IPOP):
      if (opstack.empty()) perror("Can't pop from an empty stack");
      opstack.pop(); break;
    case(Instruction::IDUP):
      if (opstack.empty()) perror("Can't dup from an empty stack");
      opstack.push(opstack.top()); break;
    case(Instruction::IPRINT): print_stack(); break;
    case(Instruction::ISKIP): break;
    default: perror("Programming Error 1");
    }
    pc++;
  } else if (itype==Instruction::IPUSH || itype==Instruction::ISTORE ||
	     itype==Instruction::ILOAD) {
    int arg = instr->argint;
    switch (itype) {
    case(Instruction::IPUSH): opstack.push(instr->argint); break;
    case (Instruction::ISTORE):
      if (opstack.empty()) perror("Can't store from an empty stack");
      register_write(instr->argint, opstack.top()); opstack.pop(); break;
      break;
    case(Instruction::ILOAD):
      opstack.push(register_read(instr->argint)); break;
      break;
    default: perror("Programming Error 2");
    }
    pc++;
  } else if (itype==Instruction::IJMPEQ || itype==Instruction::IJMPGT ||
	     itype==Instruction::IJMPGE || itype==Instruction::IJMPLT ||
	     itype==Instruction::IJMPLE) {
    bool jump = false;
    top = opstack.top(); opstack.pop();
    next = opstack.top(); opstack.pop();    
    switch(itype) {
    case(Instruction::IJMPEQ): jump = (next==top); break;
    case(Instruction::IJMPGT): jump = (next>top); break;
    case(Instruction::IJMPGE): jump = (next>=top); break;
    case(Instruction::IJMPLT): jump = (next<top); break;
    case(Instruction::IJMPLE): jump = (next<=top); break;
    default: perror("Programming Error 3");
    }
    if (jump) pc=instr->argint; else pc++;
  } else if (itype==Instruction::IADD || itype==Instruction::ISUB || itype==Instruction::IMUL
	     || itype==Instruction::IDIV || itype==Instruction::ISWAP) {
    top = opstack.top(); opstack.pop();
    next = opstack.top(); opstack.pop();    
    switch(itype) {
    case(Instruction::IADD): opstack.push(next+top); break;
    case(Instruction::ISUB): opstack.push(next-top); break;
    case(Instruction::IMUL): opstack.push(next*top); break;
    case(Instruction::IDIV): opstack.push(next/top); break;
    case(Instruction::ISWAP): opstack.push(top); opstack.push(next); break;
    default: perror("Programming Error 4");
    }
    pc++;
  } else if (itype == Instruction::IGOTO) {
    pc = instr->argint;
  } else {
    cout << "Programming Error: execute instruction" << endl;
    exit(0);
  }
}

void SVM::print_stack() {
  stack<int> local;
  cout << "stack [ ";
  while(!opstack.empty()) {
    cout << opstack.top() << " ";
    local.push(opstack.top());
    opstack.pop();
  }
  cout << "]" << endl;
  while(!local.empty()) {
    opstack.push(local.top());
    local.pop();
  }
}

void SVM::print() {
  for(int i= 0; i < instructions.size(); i++) {
    Instruction* s = instructions[i];
    if (s->label != "")
      cout << s->label << ": ";
    cout << snames[s->type] << " ";
    if (s->hasarg) {
      if (s->jmplabel == "")
	cout << s->argint;
      else {
	cout << s->jmplabel;
	// cout << "  [" << s->argint << "]";
      }
    }
    cout << endl;
  }					    
}


void SVM::register_write(int r,int v) {
  if (r > 7 || r < 0)
    perror("Invalid register number");
  registers[r] = v;
}
  
int SVM::register_read(int r) {
  if (r > 7 || r < 0)
    perror("Invalid register number");
  return registers[r];
}

    
void SVM::perror(string msg) {
  cout << "error: " << msg << endl;
  exit(0);
}



  





