class TangetTuple {
	protected vector _inTangent;
	protected vector _outTangent;
	
	//------------------------------------------------------------------------------------------------
	// constructor
	void TangetTuple(vector inTan, vector outTan)
	{
		_inTangent = inTan;
		_outTangent = outTan;
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetInTangent() {return _inTangent;}
	
	//------------------------------------------------------------------------------------------------
	vector GetOutTangent() {return _outTangent;}
	
	//------------------------------------------------------------------------------------------------
	string toString()
	{
		return "[" + _inTangent.ToString() + ", " + _outTangent.ToString() + "]";
	}
}
