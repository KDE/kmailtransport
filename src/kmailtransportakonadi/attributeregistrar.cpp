/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dispatchmodeattribute.h"
#include "errorattribute.h"
#include "sentactionattribute.h"
#include "sentbehaviourattribute.h"
#include "transportattribute.h"

#include <attributefactory.h>

namespace
{
// Anonymous namespace; function is invisible outside this file.
bool dummy()
{
    using namespace Akonadi;
    using namespace MailTransport;
    AttributeFactory::registerAttribute<DispatchModeAttribute>();
    AttributeFactory::registerAttribute<ErrorAttribute>();
    AttributeFactory::registerAttribute<SentActionAttribute>();
    AttributeFactory::registerAttribute<SentBehaviourAttribute>();
    AttributeFactory::registerAttribute<TransportAttribute>();
    return true;
}

// Called when this library is loaded.
const bool registered = dummy();
} // namespace
