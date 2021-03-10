#pragma once

#include "CollectionTypes.h"

/** Does something... */
class FSpecifierCollector
{
public:

	/** Gather specifiers and output them. */
	bool GatherSpecifiers(const char* WorkingDirectory, FSpecifierCollection& OutCollection);

private:

	/** The root directory from which we begin parsing. */
	const char* _WorkingDirectory;
};
