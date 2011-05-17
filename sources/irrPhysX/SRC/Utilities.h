
#ifndef INC_UTILITIES_H
#define INC_UTILITIES_H

/**
\brief Copies an error type string into the specified character array
\param errorCode - the errorCode to return the string for
\param str - the character array to populate
\param strSize - the number of elements the string can hold
*/
void getNxSDKCreateError(const NxSDKCreateError& errorCode, c8* str, u32 strSize) {

	switch(errorCode) {
		case NXCE_NO_ERROR: strcpy_s(str, strSize, "NXCE_NO_ERROR");
		case NXCE_PHYSX_NOT_FOUND: strcpy_s(str, strSize, "NXCE_PHYSX_NOT_FOUND");
		case NXCE_WRONG_VERSION: strcpy_s(str, strSize, "NXCE_WRONG_VERSION");
		case NXCE_DESCRIPTOR_INVALID: strcpy_s(str, strSize, "NXCE_DESCRIPTOR_INVALID");
		case NXCE_CONNECTION_ERROR: strcpy_s(str, strSize, "NXCE_CONNECTION_ERROR");
		case NXCE_RESET_ERROR: strcpy_s(str, strSize, "NXCE_RESET_ERROR");
		case NXCE_IN_USE_ERROR: strcpy_s(str, strSize, "NXCE_IN_USE_ERROR");
		default: strcpy_s(str, strSize, "Unknown error");
	}

};

#endif /* INC_UTILITIES_H */
