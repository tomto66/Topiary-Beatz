

int swing(int value, int deviation, int q) 
		// q indicates whetherwe are swinging quarter or eight based
	{ 
		// value between 0 - ticksPerBeat; deviation -100 to 100
		jassert(deviation < 101);
		jassert(deviation > -101);
		jassert(value >= 0);

		int maxMidiValue;
		
		if (q == Topiary::SwingQButtonIds::SwingQ8)
			maxMidiValue = (int) (Topiary::TicksPerQuarter/2) - 1;
		else
			maxMidiValue = Topiary::TicksPerQuarter - 1;

		jassert(value <= maxMidiValue);

		int midMidiValue = (int) (maxMidiValue / 2);

		// This is our control point for the quadratic bezier curve
		// We want this to be between 0 (min) and 63.5 (max)
		double dev = (double)deviation;
		double controlPointX = (double) midMidiValue + ((dev / 100) * midMidiValue);

		// Get the percent position of the incoming value in relation to the max
		double t = (double)value / maxMidiValue;

		// The quadratic bezier curve formula
		// B(t) = ((1 - t) * (1 - t) * p0) + (2 * (1 - t) * t * p1) + (t * t * p2)

		// t  = the position on the curve between (0 and 1)
		// p0 = minMidiValue (0)
		// p1 = controlPointX (the bezier control point)
		// p2 = maxMidiValue (127)

		// Formula can now be simplified as:
		// B(t) = ((1 - t) * (1 - t) * minMidiValue) + (2 * (1 - t) * t * controlPointX) + (t * t * maxMidiValue)

		// What is the deviation from our value?
		
		int delta = (int) round((2.0 * (1 - t) * t * controlPointX) + (t * t * (double)maxMidiValue));

		//Logger::outputDebugString("swing delta: " + String(delta));
		return (value - delta) + value;
	
	} // swing