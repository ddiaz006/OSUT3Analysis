#include "OSUT3Analysis/Collections/plugins/BeamspotProducer.h"

#if IS_VALID(beamspots)

#include "OSUT3Analysis/AnaTools/interface/CommonUtils.h"

BeamspotProducer::BeamspotProducer (const edm::ParameterSet &cfg) :
  collections_ (cfg.getParameter<edm::ParameterSet> ("collections"))
{
  collection_ = collections_.getParameter<edm::InputTag> ("beamspots");

  produces<osu::Beamspot> (collection_.instance ());
}

BeamspotProducer::~BeamspotProducer ()
{
}

void
BeamspotProducer::produce (edm::Event &event,  const edm::EventSetup &setup)
{
  edm::Handle<TYPE (beamspots)> collection;
  if (!anatools::getCollection (collection_,  collection,  event))
    return;
  pl_ = auto_ptr<osu::Beamspot>  (new osu::Beamspot (*collection));
  event.put (pl_,  collection_.instance ());
  pl_.reset ();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(BeamspotProducer);

#endif