#ifndef PT_SQLITE_ERROR_H
#define PT_SQLITE_ERROR_H

#include "Pt/SourceInfo.h"


namespace Pt {

namespace Db {

namespace sqlite {

	void Error(int errorCode, const Pt::SourceInfo& srcInfo);

} // namespace sqlite

} // namespace Db

} // namespace Pt

#endif
