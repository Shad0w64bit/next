class OpCallFunction: public Operation
{
public:
	OpCallFunction(Function* func)
		: Operation(Operation::CallFunction, (char*)func) {}
		
	Function* getFunction() { return (Function*) m_op1; }
	
	std::vector<Variable*>* getArguments() { return &m_args; }
	
	void addArgument(Variable* var) { m_args.push_back(var); };
protected:
	std::vector<Variable*> m_args;
};