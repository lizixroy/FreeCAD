/***************************************************************************
 *   Copyright (c) 2010 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <App/Application.h>
#include <App/Document.h>
#include <Base/Placement.h>

#include "BodyBase.h"


namespace Part {


PROPERTY_SOURCE_WITH_EXTENSIONS(Part::BodyBase, Part::Feature)

BodyBase::BodyBase()
{
    ADD_PROPERTY(Tip         , (0) );
    ADD_PROPERTY(BaseFeature , (0) );
    
    App::OriginGroupExtension::initExtension(this);
}

BodyBase* BodyBase::findBodyOf(const App::DocumentObject* f)
{
    App::Document* doc = f->getDocument();
    if (doc != NULL) {
        std::vector<App::DocumentObject*> bodies = doc->getObjectsOfType(BodyBase::getClassTypeId());
        for (std::vector<App::DocumentObject*>::const_iterator b = bodies.begin(); b != bodies.end(); b++) {
            BodyBase* body = static_cast<BodyBase*>(*b);
            if (body->hasObject(f))
                return body;
        }
    }

    return NULL;
}

bool BodyBase::isAfter(const App::DocumentObject *feature, const App::DocumentObject* target) const {
    assert (feature);

    if (feature == target) {
        return false;
    }

    if (!target || target == BaseFeature.getValue() ) {
        return hasObject (feature);
    }

    const std::vector<App::DocumentObject *> & features = Group.getValues();
    auto featureIt = std::find(features.begin(), features.end(), feature);
    auto targetIt = std::find(features.begin(), features.end(), target);

    if (featureIt == features.end()) {
        return false;
    } else {
        return featureIt > targetIt;
    }
}

void BodyBase::onBeforeChange (const App::Property* prop) {
    // If we are changing the base feature and tip point to it reset it
    if ( prop == &BaseFeature && BaseFeature.getValue() == Tip.getValue() && BaseFeature.getValue() ) {
        Tip.setValue( nullptr );
    }
    Part::Feature::onBeforeChange ( prop );
}

void BodyBase::onChanged (const App::Property* prop) {
    // If the tip is zero and we are adding a base feature to the body set it to be the tip
    if ( prop == &BaseFeature && !Tip.getValue() && BaseFeature.getValue() ) {
        Tip.setValue( BaseFeature.getValue () );
    }
    Part::Feature::onChanged ( prop );
}

} /* Part */
