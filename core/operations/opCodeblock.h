class OpCodeblock: public Operation
{
public:
	OpCodeblock(Codeblock* cb)
		: Operation(Operation::Type::CodeBlock, (char*)cb) {}
		
	Codeblock* getCodeblock() { return (Codeblock*) m_op1; }
};