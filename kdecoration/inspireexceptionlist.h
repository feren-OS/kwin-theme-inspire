#ifndef inspireexceptionlist_h
#define inspireexceptionlist_h

//////////////////////////////////////////////////////////////////////////////
// inspireexceptionlist.h
// window decoration exceptions
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "inspiresettings.h"
#include "inspire.h"

#include <KSharedConfig>

namespace Inspire
{

    //! inspire exceptions list
    class ExceptionList
    {

        public:

        //! constructor from list
        explicit ExceptionList( const InternalSettingsList& exceptions = InternalSettingsList() ):
            _exceptions( exceptions )
        {}

        //! exceptions
        const InternalSettingsList& get( void ) const
        { return _exceptions; }

        //! read from KConfig
        void readConfig( KSharedConfig::Ptr );

        //! write to kconfig
        void writeConfig( KSharedConfig::Ptr );

        protected:

        //! generate exception group name for given exception index
        static QString exceptionGroupName( int index );

        //! read configuration
        static void readConfig( KCoreConfigSkeleton*, KConfig*, const QString& );

        //! write configuration
        static void writeConfig( KCoreConfigSkeleton*, KConfig*, const QString& );

        private:

        //! exceptions
        InternalSettingsList _exceptions;

    };

}

#endif
