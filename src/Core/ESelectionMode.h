#ifndef ESELECTIONMODE_H
#define ESELECTIONMODE_H

///----------------------------------------------------------------------------------------------------
/// ESelectionMode Enumeration
///----------------------------------------------------------------------------------------------------
enum class ESelectionMode
{
	None,
	Click,
	Release,
	ReleaseOrClick,
	Hover,

	Escape,    /* internal */
	SingleItem /* internal */
};

#endif
