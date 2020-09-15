void recalcRealTime()
	{ // housekeeping in case sampleRate and/or BPM changes
		// BPM/60 = beatPerSecond
		// lenght of 1 beat depends on denumerator (if 4 then beat == quarter)
		if (numerator == 0) return; // not initialized!
		if (denominator == 0) return;
		ticksPerBeat = Topiary::TicksPerQuarter * 4 / denominator;
		samplesPerTick = (double)sampleRate / ((double)ticksPerBeat * BPM / 60.0);
		Log("Samples per tick: " + String(samplesPerTick), Topiary::LogType::Info);

	} // recalcRealTime