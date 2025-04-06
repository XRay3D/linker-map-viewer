#pragma once
#include <QDebug>

public: interface MemoryConfigurationSelectionListener
{
	public: void selectionChanged (bool[] memoryConfigurationEnables);
};
