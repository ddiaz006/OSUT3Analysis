#include "OSUT3Analysis/Collections/interface/Track.h"

#if IS_VALID(tracks)

osu::Track::Track () :
  dRMinJet_ (INVALID_VALUE),
  isFiducialElectronTrack_ (true),
  isFiducialMuonTrack_ (true),
  maxSigmaForFiducialElectronTrack_ (-1.0),
  maxSigmaForFiducialMuonTrack_ (-1.0),
  matchedGsfTrack_ (),
  dRToMatchedGsfTrack_ (INVALID_VALUE),
  maxDeltaR_ (-1.0),
  EcalAllDeadChannelsValMap_ (NULL),
  EcalAllDeadChannelsBitMap_ (NULL),
  isFiducialECALTrack_ (true),
  dropTOBDecision_ (-1.0),
  dropHitDecisions_ ({}),
  dropMiddleHitDecisions_ ({})
{
}

osu::Track::Track (const TYPE(tracks) &track) :
  GenMatchable (track),
  dRMinJet_ (INVALID_VALUE),
  isFiducialElectronTrack_ (true),
  isFiducialMuonTrack_ (true),
  maxSigmaForFiducialElectronTrack_ (-1.0),
  maxSigmaForFiducialMuonTrack_ (-1.0),
  matchedGsfTrack_ (),
  dRToMatchedGsfTrack_ (INVALID_VALUE),
  maxDeltaR_ (-1.0),
  EcalAllDeadChannelsValMap_ (NULL),
  EcalAllDeadChannelsBitMap_ (NULL),
  isFiducialECALTrack_ (true),
  dropTOBDecision_ (-1.0),
  dropHitDecisions_ ({}),
  dropMiddleHitDecisions_ ({})
{
}

osu::Track::Track (const TYPE(tracks) &track, const edm::Handle<vector<osu::Mcparticle> > &particles) :
  GenMatchable (track, particles),
  dRMinJet_ (INVALID_VALUE),
  isFiducialElectronTrack_ (true),
  isFiducialMuonTrack_ (true),
  maxSigmaForFiducialElectronTrack_ (-1.0),
  maxSigmaForFiducialMuonTrack_ (-1.0),
  matchedGsfTrack_ (),
  dRToMatchedGsfTrack_ (INVALID_VALUE),
  maxDeltaR_ (-1.0),
  EcalAllDeadChannelsValMap_ (NULL),
  EcalAllDeadChannelsBitMap_ (NULL),
  isFiducialECALTrack_ (true),
  dropTOBDecision_ (-1.0),
  dropHitDecisions_ ({}),
  dropMiddleHitDecisions_ ({})
{
}

osu::Track::Track (const TYPE(tracks) &track, const edm::Handle<vector<osu::Mcparticle> > &particles, const edm::ParameterSet &cfg) :
  GenMatchable (track, particles, cfg),
  dRMinJet_ (INVALID_VALUE),
  isFiducialElectronTrack_ (true),
  isFiducialMuonTrack_ (true),
  maxSigmaForFiducialElectronTrack_ (-1.0),
  maxSigmaForFiducialMuonTrack_ (-1.0),
  matchedGsfTrack_ (),
  dRToMatchedGsfTrack_ (INVALID_VALUE),
  maxDeltaR_ (-1.0),
  EcalAllDeadChannelsValMap_ (NULL),
  EcalAllDeadChannelsBitMap_ (NULL),
  isFiducialECALTrack_ (true),
  dropTOBDecision_ (-1.0),
  dropHitDecisions_ ({}),
  dropMiddleHitDecisions_ ({})
{
}

osu::Track::Track (const TYPE(tracks) &track, const edm::Handle<vector<osu::Mcparticle> > &particles, const edm::ParameterSet &cfg, const edm::Handle<vector<reco::GsfTrack> > &gsfTracks, const EtaPhiList &electronVetoList, const EtaPhiList &muonVetoList, const map<DetId, vector<double> > * const EcalAllDeadChannelsValMap, const map<DetId, vector<int> > * const EcalAllDeadChannelsBitMap, const bool dropHits) :
  GenMatchable (track, particles, cfg),
  dRMinJet_ (INVALID_VALUE),
  minDeltaRForFiducialTrack_ (cfg.getParameter<double> ("minDeltaRForFiducialTrack")),
  isFiducialElectronTrack_ (isFiducialTrack (electronVetoList, minDeltaRForFiducialTrack_, maxSigmaForFiducialElectronTrack_)),
  isFiducialMuonTrack_ (isFiducialTrack (muonVetoList, minDeltaRForFiducialTrack_, maxSigmaForFiducialMuonTrack_)),
  EcalAllDeadChannelsValMap_ (EcalAllDeadChannelsValMap),
  EcalAllDeadChannelsBitMap_ (EcalAllDeadChannelsBitMap),
  isFiducialECALTrack_ (!isCloseToBadEcalChannel (minDeltaRForFiducialTrack_)),
  dropTOBDecision_ (-1.0),
  dropHitDecisions_ ({}),
  dropMiddleHitDecisions_ ({})
{
  maxDeltaR_ = cfg.getParameter<double> ("maxDeltaRForGsfTrackMatching");
  if (gsfTracks.isValid ())
    findMatchedGsfTrack (gsfTracks, matchedGsfTrack_, dRToMatchedGsfTrack_);
  EcalAllDeadChannelsValMap_ = NULL;
  EcalAllDeadChannelsBitMap_ = NULL;

  dropTOBProbability_ = cfg.getParameter<double> ("dropTOBProbability");
  preTOBDropHitProbability_ = cfg.getParameter<double> ("preTOBDropHitInefficiency");
  postTOBDropHitProbability_ = cfg.getParameter<double> ("postTOBDropHitInefficiency");
  hitProbability_ = cfg.getParameter<double> ("hitInefficiency");

  stringstream ss;
  ss  <<  "dropTOBProbability:         "  <<  (dropTOBProbability_         *  100.0)  <<  "%"   <<  endl
      <<  "preTOBDropHitProbability:   "  <<  (preTOBDropHitProbability_   *  100.0)  <<  "%"   <<  endl
      <<  "postTOBDropHitProbability:  "  <<  (postTOBDropHitProbability_  *  100.0)  <<  "%"   <<  endl
      <<  "hitProbability:             "  <<  (hitProbability_             *  100.0)  <<  "%";
  edm::LogInfo ("osu_Track") << ss.str ();

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator (seed);
  uniform_real_distribution<double> distribution (0.0, 1.0);
  dropTOBDecision_ = (dropHits ? distribution (generator) : 1.0e6) < dropTOBProbability_;
  for (int i = 0; i < 50; i++)
    dropHitDecisions_.push_back ((dropHits ? distribution (generator) : 1.0e6) < (dropTOBDecision_ ? postTOBDropHitProbability_ : preTOBDropHitProbability_));
  for (int i = 0; i < 50; i++)
    dropMiddleHitDecisions_.push_back ((dropHits ? distribution (generator) : 1.0e6) < hitProbability_);

  // PrintTrackHitPatternInfo();

}

osu::Track::~Track ()
{
}

const bool
osu::Track::isFiducialElectronTrack () const
{
  return isFiducialElectronTrack_;
}

const bool
osu::Track::isFiducialMuonTrack () const
{
  return isFiducialMuonTrack_;
}

const bool
osu::Track::isFiducialECALTrack () const
{
  return isFiducialECALTrack_;
}

const double
osu::Track::maxSigmaForFiducialElectronTrack () const
{
  return maxSigmaForFiducialElectronTrack_;
}

const double
osu::Track::maxSigmaForFiducialMuonTrack () const
{
  return maxSigmaForFiducialMuonTrack_;
}

const edm::Ref<vector<reco::GsfTrack> >
osu::Track::matchedGsfTrack () const
{
  return matchedGsfTrack_;
}

const double
osu::Track::dRToMatchedGsfTrack () const
{
  if (IS_INVALID(dRToMatchedGsfTrack_))
    return MAX_DR;
  return dRToMatchedGsfTrack_;
}

const double
osu::Track::dRMinJet () const
{
  if (IS_INVALID(dRMinJet_))
    return MAX_DR;
  return dRMinJet_;
}

void
osu::Track::set_dRMinJet(const double dRMinJet)
{
  dRMinJet_ = dRMinJet;
}

const bool
osu::Track::isFiducialTrack (const EtaPhiList &vetoList, const double minDeltaR, double &maxSigma) const
{
  const double minDR = max (minDeltaR, vetoList.minDeltaR); // use the given parameter unless the bin size from which the veto list is calculated is larger
  bool isFiducial = true;
  maxSigma = 0.0;
  for (const auto &etaPhi : vetoList)
    {
      if (deltaR (this->eta (), this->phi (), etaPhi.eta, etaPhi.phi) < minDR)
        {
          isFiducial = false;
          if (etaPhi.sigma > maxSigma)
            maxSigma = etaPhi.sigma;
        }
    }
  return isFiducial;
}

const edm::Ref<vector<reco::GsfTrack> > &
osu::Track::findMatchedGsfTrack (const edm::Handle<vector<reco::GsfTrack> > &gsfTracks, edm::Ref<vector<reco::GsfTrack> > &matchedGsfTrack, double &dRToMatchedGsfTrack) const
{
  dRToMatchedGsfTrack = INVALID_VALUE;
  for (vector<reco::GsfTrack>::const_iterator gsfTrack = gsfTracks->begin (); gsfTrack != gsfTracks->end (); gsfTrack++)
    {
      double dR = deltaR (*gsfTrack, *this);
      if (maxDeltaR_ >= 0.0 && dR > maxDeltaR_)
        continue;
      if (dR < dRToMatchedGsfTrack || dRToMatchedGsfTrack < 0.0)
        {
          dRToMatchedGsfTrack = dR;
          matchedGsfTrack = edm::Ref<vector<reco::GsfTrack> > (gsfTracks, gsfTrack - gsfTracks->begin ());
        }
    }

  return matchedGsfTrack;
}

const int
osu::Track::gsfTrackNumberOfValidHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->numberOfValidHits ();

  return INVALID_VALUE;
}

const int
osu::Track::gsfTrackNumberOfValidPixelHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->hitPattern().numberOfValidPixelHits ();

  return INVALID_VALUE;
}

const int
osu::Track::gsfTrackNumberOfValidPixelBarrelHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->hitPattern().numberOfValidPixelBarrelHits ();

  return INVALID_VALUE;
}

const int
osu::Track::gsfTrackNumberOfValidPixelEndcapHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->hitPattern().numberOfValidPixelEndcapHits ();

  return INVALID_VALUE;
}

const int
osu::Track::gsfTrackMissingInnerHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_INNER_HITS);

  return INVALID_VALUE;
}

const int
osu::Track::gsfTrackMissingMiddleHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::TRACK_HITS);

  return INVALID_VALUE;
}

const int
osu::Track::gsfTrackMissingOuterHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    return this->matchedGsfTrack_->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_OUTER_HITS);

  return INVALID_VALUE;
}

const int
osu::Track::bestTrackNumberOfValidHits () const
{
  int nHits = gsfTrackNumberOfValidHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->numberOfValidHits ();

  return nHits;
}

const int
osu::Track::bestTrackNumberOfValidPixelHits () const
{
  int nHits = gsfTrackNumberOfValidPixelHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->hitPattern ().numberOfValidPixelHits ();

  return nHits;
}

const int
osu::Track::bestTrackNumberOfValidPixelBarrelHits () const
{
  int nHits = gsfTrackNumberOfValidPixelBarrelHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->hitPattern ().numberOfValidPixelBarrelHits ();

  return nHits;
}

const int
osu::Track::bestTrackNumberOfValidPixelEndcapHits () const
{
  int nHits = gsfTrackNumberOfValidPixelEndcapHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->hitPattern ().numberOfValidPixelEndcapHits ();

  return nHits;
}

const int
osu::Track::bestTrackMissingInnerHits () const
{
  int nHits = gsfTrackMissingInnerHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_INNER_HITS);

  return nHits;
}

const int
osu::Track::bestTrackMissingMiddleHits () const
{
  int nHits = gsfTrackMissingMiddleHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::TRACK_HITS);

  return nHits;
}

const int
osu::Track::bestTrackMissingOuterHits () const
{
  int nHits = gsfTrackMissingOuterHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = this->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_OUTER_HITS);

  return nHits;
}

/*******************************************************************************
 * Methods for testing effect of dropping random hits and all the TOB hits.
*******************************************************************************/

/* Missing middle hits */

template<class T> const int
osu::Track::extraMissingMiddleHits (const T &track) const
{
  int nHits = 0;
  bool countMissingMiddleHits = false;
  for (int i = 0; i < track.hitPattern ().stripLayersWithMeasurement () - (dropTOBDecision_ ? this->hitPattern ().stripTOBLayersWithMeasurement () : 0); i++)
    {
      bool hit = !dropMiddleHitDecisions_.at (i);
      if (!hit && countMissingMiddleHits)
        nHits++;
      if (hit)
        countMissingMiddleHits = true;
    }

  return nHits;
}

const int
osu::Track::hitDrop_missingMiddleHits () const
{
  int nDropHits = extraMissingMiddleHits (*this);
  return this->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::TRACK_HITS) + nDropHits;
}

const int
osu::Track::hitDrop_gsfTrackMissingMiddleHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    {
      int nDropHits = extraMissingMiddleHits (*this->matchedGsfTrack_);
      return this->matchedGsfTrack_->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::TRACK_HITS) + nDropHits;
    }

  return INVALID_VALUE;
}

const int
osu::Track::hitDrop_bestTrackMissingMiddleHits () const
{
  int nHits = hitDrop_gsfTrackMissingMiddleHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = hitDrop_missingMiddleHits ();

  return nHits;
}

/* Missing outer hits */

template<class T> const int
osu::Track::extraMissingOuterHits (const T &track) const
{
  int nHits = 0;
  for (int i = 0; i < track.hitPattern ().stripLayersWithMeasurement () - (dropTOBDecision_ ? this->hitPattern ().stripTOBLayersWithMeasurement () : 0); i++)
    {
      bool hit = !dropHitDecisions_.at (i);
      if (!hit)
        nHits++;
      else
        break;
    }

  return nHits;
}

const int
osu::Track::hitAndTOBDrop_missingOuterHits () const
{
  int nDropTOBHits = (dropTOBDecision_ ? this->hitPattern ().stripTOBLayersWithMeasurement () : 0);
  int nDropHits = extraMissingOuterHits (*this);
  return this->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_OUTER_HITS) + nDropTOBHits + nDropHits;
}

const int
osu::Track::hitAndTOBDrop_gsfTrackMissingOuterHits () const
{
  if (this->matchedGsfTrack_.isNonnull ())
    {
      int nDropTOBHits = (dropTOBDecision_ ? this->matchedGsfTrack_->hitPattern ().stripTOBLayersWithMeasurement () : 0);
      int nDropHits = extraMissingOuterHits (*this->matchedGsfTrack_);
      return this->matchedGsfTrack_->hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_OUTER_HITS) + nDropTOBHits + nDropHits;
    }

  return INVALID_VALUE;
}

const int
osu::Track::hitAndTOBDrop_bestTrackMissingOuterHits () const
{
  int nHits = hitAndTOBDrop_gsfTrackMissingOuterHits ();
  if (IS_INVALID(nHits) || isBadGsfTrack (*this->matchedGsfTrack_))
    nHits = hitAndTOBDrop_missingOuterHits ();

  return nHits;
}

/******************************************************************************/
// Debug HitPattern methods
/******************************************************************************/

// From DataFormats/TrackReco/interface/HitPattern.h:
//
// Hit pattern is the summary information of the hits associated to track in
// AOD.  When RecHits are no longer available, the compact hit pattern should
// allow basic track selection based on the hits in various subdetectors.
// The hits of a track are saved in unit16_t hitPattern[MaxHits].
//
//                                            uint16_t
// +--------+---------------+---------------------------+-----------------+----------------+
// |  tk/mu | sub-structure |     sub-sub-structure     |     stereo      |    hit type    |
// +--------+---------------+---------------------------+-----------------+----------------+
// |   10   | 9   8    7    |  6     5     4     3      |        2        |    1        0  |  bit
// +--------+---------------+---------------------------+-----------------+----------------|
// | tk = 1 |    PXB = 1    | layer = 1-3               |                 | hit type = 0-3 |
// | tk = 1 |    PXF = 2    | disk  = 1-2               |                 | hit type = 0-3 |
// | tk = 1 |    TIB = 3    | layer = 1-4               | 0=rphi,1=stereo | hit type = 0-3 |
// | tk = 1 |    TID = 4    | wheel = 1-3               | 0=rphi,1=stereo | hit type = 0-3 |
// | tk = 1 |    TOB = 5    | layer = 1-6               | 0=rphi,1=stereo | hit type = 0-3 |
// | tk = 1 |    TEC = 6    | wheel = 1-9               | 0=rphi,1=stereo | hit type = 0-3 |
// | mu = 0 |    DT  = 1    | 4*(stat-1)+superlayer     |                 | hit type = 0-3 |
// | mu = 0 |    CSC = 2    | 4*(stat-1)+(ring-1)       |                 | hit type = 0-3 |
// | mu = 0 |    RPC = 3    | 4*(stat-1)+2*layer+region |                 | hit type = 0-3 |
// | mu = 0 |    GEM = 4    | 2*(stat-1)+2*(layer-1)    |                 | hit type = 0-3 |
// | mu = 0 |    ME0 = 5    | roll                      |                 | hit type = 0-3 |
// +--------+---------------+---------------------------+-----------------+----------------+
//
//  hit type, see DataFormats/TrackingRecHit/interface/TrackingRecHit.h
//      VALID    = valid hit                                     = 0
//      MISSING  = detector is good, but no rec hit found        = 1
//      INACTIVE = detector is off, so there was no hope         = 2
//      BAD      = there were many bad strips within the ellipse = 3

void
osu::Track::PrintTrackHitCategoryPatterns (const reco::HitPattern::HitCategory category) const
{

  std::cout << A_BRIGHT_CYAN;
  if(category == reco::HitPattern::TRACK_HITS) std::cout << "TRACK_HITS:";
  else if(category == reco::HitPattern::MISSING_INNER_HITS) std::cout << "MISSING_INNER_HITS:";
  else if(category == reco::HitPattern::MISSING_OUTER_HITS) std::cout << "MISSING_OUTER_HITS:";
  std::cout << A_RESET << std::endl;

  const reco::HitPattern &p = this->hitPattern();

  for (int i = 0; i < p.numberOfHits(category); i++) {

    uint16_t hit = p.getHitPattern(category, i);

    uint32_t subDetector = reco::HitPattern::getSubDetector(hit);
    uint32_t subStructure = reco::HitPattern::getSubStructure(hit);
    uint32_t subSubStructure = reco::HitPattern::getSubSubStructure(hit);
    uint32_t side = reco::HitPattern::getSide(hit);
    uint32_t hitType = reco::HitPattern::getHitType(hit);

    if(subDetector == 1) { // tk
      std::cout << "TK ";
      if(subStructure == 1) std::cout << "PXB layer " << subSubStructure << " ";
      if(subStructure == 2) std::cout << "PXF disk " << subSubStructure << " ";
      if(subStructure == 3) std::cout << "TIB layer " << subSubStructure << (side == 0 ? " (mono) " : " (stereo) ");
      if(subStructure == 4) std::cout << "TID wheel " << subSubStructure << (side == 0 ? " (mono) " : " (stereo) ");
      if(subStructure == 5) std::cout << "TOB layer " << subSubStructure << (side == 0 ? " (mono) " : " (stereo) ");
      if(subStructure == 6) std::cout << "TEC wheel " << subSubStructure << (side == 0 ? " (mono) " : " (stereo) ");
    }

    if(subDetector == 0) { // mu

      std::cout << "MU ";
      uint16_t station = reco::HitPattern::getMuonStation(hit);

      if(subStructure == 1) {
        uint16_t dtSuperLayer = reco::HitPattern::getDTSuperLayer(hit);
        std::cout << "DT station " << station;
        if(dtSuperLayer == 0) std::cout << " (segment) ";
        else std::cout << " (superlayer " << dtSuperLayer << ") ";
      }
      if(subStructure == 2) std::cout << "CSC station " << station << ", layer " << reco::HitPattern::getCSCRing(hit);
      if(subStructure == 3) {
        uint16_t rpcLayer = reco::HitPattern::getRPCLayer(hit);
        std::cout << "RPC station " << station;
        if(station == 1 || station == 2) std::cout << (rpcLayer == 1 ? " (inner layer) " : " (outer layer) ");
        std::cout << (reco::HitPattern::getRPCregion(hit) == 0 ? " barrel " : " endcap ") << " region ";
      }
      if(subStructure == 4) std::cout << "GEM station " << reco::HitPattern::getGEMStation(hit) << " layer " << reco::HitPattern::getGEMLayer(hit);
      if(subStructure == 5) std::cout << "ME0 ";
    }

    std::cout << "-- ";

    if(hitType == 0) std::cout << A_BRIGHT_GREEN << "VALID" << A_RESET << std::endl;
    if(hitType == 1) std::cout << A_BRIGHT_RED << "MISSING" << A_RESET << std::endl;
    if(hitType == 2) std::cout << A_BRIGHT_RED << "INACTIVE" << A_RESET << std::endl;
    if(hitType == 3) std::cout << A_BRIGHT_RED << "BAD" << A_RESET << std::endl;

  } // end loop over category

}

void
osu::Track::PrintTrackHitPatternInfo () const
{

  std::cout << std::endl;
  std::cout << "=========== Track HitPattern ===========" << std::endl;
  PrintTrackHitCategoryPatterns(reco::HitPattern::TRACK_HITS);
  PrintTrackHitCategoryPatterns(reco::HitPattern::MISSING_INNER_HITS);
  PrintTrackHitCategoryPatterns(reco::HitPattern::MISSING_OUTER_HITS);
  std::cout << std::endl;
  std::cout << "numberOfValidHits = " << this->bestTrackNumberOfValidHits() << std::endl;
  std::cout << "missingInnerHits = "  << this->hitPattern().trackerLayersWithoutMeasurement(reco::HitPattern::MISSING_INNER_HITS) << std::endl;
  std::cout << "missingMiddleHits = " << this->hitPattern().trackerLayersWithoutMeasurement(reco::HitPattern::TRACK_HITS) << std::endl;
  std::cout << "missingOuterHits = "  << this->hitPattern().trackerLayersWithoutMeasurement(reco::HitPattern::MISSING_OUTER_HITS) << std::endl;
  std::cout << "========================================" << std::endl << std::endl;

}

const bool
osu::Track::hasValidHitInPixelBarrelLayer (const uint16_t layer) const
{

  const reco::HitPattern &p = this->hitPattern();

  // Loop over TRACK_HITS
  for (int i = 0; i < p.numberOfHits(reco::HitPattern::TRACK_HITS); i++) {
    uint16_t hit = p.getHitPattern(reco::HitPattern::TRACK_HITS, i);
    if(reco::HitPattern::pixelBarrelHitFilter(hit) &&
       reco::HitPattern::getLayer(hit) == layer &&
       reco::HitPattern::validHitFilter(hit)) {
         return true;
     }
   }

   return false;
 }

const bool
osu::Track::hasValidHitInPixelEndcapLayer (const uint16_t layer) const
{

  const reco::HitPattern &p = this->hitPattern();

  // Loop over TRACK_HITS
  for (int i = 0; i < p.numberOfHits(reco::HitPattern::TRACK_HITS); i++) {
    uint16_t hit = p.getHitPattern(reco::HitPattern::TRACK_HITS, i);
    if(reco::HitPattern::pixelEndcapHitFilter(hit) &&
       reco::HitPattern::getLayer(hit) == layer &&
       reco::HitPattern::validHitFilter(hit)) {
         return true;
     }
   }

   return false;
}

const uint16_t
osu::Track::packedPixelBarrelHitPattern () const
{

  // Output format:
  // +--------+--------+--------+
  // |  8 7 6 |  5 4 3 |  2 1 0 | bit
  // +--------+--------+--------+
  // | status | status | status |
  // |  PXB3  |  PXB2  |  PXB1  |
  // +--------+--------+--------+
  // where status:
  // 0 = valid
  // 1 = missing
  // 2 = inactive
  // 3 = bad
  // 4 = no hit present in HitPattern at all
  // 5 = found multiple hits somehow

  // This looks at TRACK_HITS, MISSING_INNER_HITS, and MISSING_OUTER_HITS

  uint8_t statusPXB1 = 0x4;
  uint8_t statusPXB2 = 0x4;
  uint8_t statusPXB3 = 0x4;

  const reco::HitPattern &p = this->hitPattern();

  const std::array<reco::HitPattern::HitCategory, 3> categories = {{reco::HitPattern::TRACK_HITS, reco::HitPattern::MISSING_INNER_HITS, reco::HitPattern::MISSING_OUTER_HITS}};

  // Loop over TRACK_HITS, MISSING_INNER_HITS, and MISSING_OUTER_HITS
  for (auto category : categories) {

    for (int i = 0; i < p.numberOfHits(category); i++) {
      uint16_t hit = p.getHitPattern(category, i);

      if(reco::HitPattern::pixelBarrelHitFilter(hit)) {

        uint32_t layer = reco::HitPattern::getLayer(hit);

        if(layer == 1) {
          if(statusPXB1 != 0x4) { // if you already found a hit, mark this as having multiple hits
            statusPXB1 = 0x5;
            continue;
          }
          else statusPXB1 = reco::HitPattern::getHitType(hit);
        }

        if(layer == 2) {
          if(statusPXB2 != 0x4) { // if you already found a hit, mark this as having multiple hits
            statusPXB2 = 0x5;
            continue;
          }
          else statusPXB2 = reco::HitPattern::getHitType(hit);
        }

        if(layer == 3) {
          if(statusPXB3 != 0x4) { // if you already found a hit, mark this as having multiple hits
            statusPXB3 = 0x5;
            continue;
          }
          else statusPXB3 = reco::HitPattern::getHitType(hit);
        }

      } // if pixel barrel hit

    } // loop over hits in category

  } // loop over categories

  // Now pack these into a single return value

  uint16_t result = 0;

  result |= (statusPXB1 << 0);
  result |= (statusPXB2 << 3);
  result |= (statusPXB3 << 6);

  return result;

}

const uint16_t
osu::Track::packedPixelEndcapHitPattern () const
{

  // Output format:
  // +--------+--------+
  // | 5 4 3  |  2 1 0 | bit
  // +--------+--------+
  // | status | status |
  // |  PXF2  |  PXF1  |
  // +--------+--------+
  // where status:
  // 0 = valid
  // 1 = missing
  // 2 = inactive
  // 3 = bad
  // 4 = no hit present in HitPattern at all
  // 5 = found multiple hits somehow

  // This looks at TRACK_HITS, MISSING_INNER_HITS, and MISSING_OUTER_HITS

  uint8_t statusPXF1 = 0x4;
  uint8_t statusPXF2 = 0x4;

  const reco::HitPattern &p = this->hitPattern();

  const std::array<reco::HitPattern::HitCategory, 3> categories = {{reco::HitPattern::TRACK_HITS, reco::HitPattern::MISSING_INNER_HITS, reco::HitPattern::MISSING_OUTER_HITS}};

  // Loop over TRACK_HITS, MISSING_INNER_HITS, and MISSING_OUTER_HITS
  for (auto category : categories) {

    for (int i = 0; i < p.numberOfHits(category); i++) {
      uint16_t hit = p.getHitPattern(category, i);

      if(reco::HitPattern::pixelEndcapHitFilter(hit)) {

        uint32_t layer = reco::HitPattern::getLayer(hit);

        if(layer == 1) {
          if(statusPXF1 != 0x4) { // if you already found a hit, mark this as having multiple hits
            statusPXF1 = 0x5;
            continue;
          }
          else statusPXF1 = reco::HitPattern::getHitType(hit);
        }

        if(layer == 2) {
          if(statusPXF2 != 0x4) { // if you already found a hit, mark this as having multiple hits
            statusPXF2 = 0x5;
            continue;
          }
          else statusPXF2 = reco::HitPattern::getHitType(hit);
        }

      } // if pixel endcap hit

    } // loop over hits in category

  } // loop over categories

  // Now pack these into a single return value

  uint16_t result = 0;

  result |= (statusPXF1 << 0);
  result |= (statusPXF2 << 3);

  return result;

}

const uint16_t
osu::Track::firstLayerWithValidHit () const
{

  bool foundAValidHit = false;
  uint16_t earliestValidHit = 0;

  const reco::HitPattern &p = this->hitPattern();

  // loop over the TRACK_HITS of the track
  for (int i = 0; i < p.numberOfHits(reco::HitPattern::TRACK_HITS); i++) {

    uint16_t pattern = p.getHitPattern(reco::HitPattern::TRACK_HITS, i);
    uint16_t hitType = (pattern >> 0) & 0x3; // results in just the hit type bits

    pattern = (pattern - (1 << 10)) >> 3; // results in just the structure/subStructure/subSubStructure bits

    if(hitType == reco::HitPattern::HIT_TYPE::VALID) {
      if(!foundAValidHit) {
        foundAValidHit = true;
        earliestValidHit = pattern;
      }
      else if(pattern < earliestValidHit) earliestValidHit = pattern;
    }

  }

  return earliestValidHit;

}

const uint16_t
osu::Track::lastLayerWithValidHit () const
{

  bool foundAValidHit = false;
  uint16_t latestValidHit = 0;

  const reco::HitPattern &p = this->hitPattern();

  // loop over the TRACK_HITS of the track
  for (int i = 0; i < p.numberOfHits(reco::HitPattern::TRACK_HITS); i++) {

    uint16_t pattern = p.getHitPattern(reco::HitPattern::TRACK_HITS, i);
    uint16_t hitType = (pattern >> 0) & 0x3; // results in just the hit type bits

    pattern = (pattern - (1 << 10)) >> 3; // results in just the structure/subStructure/subSubStructure bits

    if(hitType == reco::HitPattern::HIT_TYPE::VALID) {
      if(!foundAValidHit) {
        foundAValidHit = true;
        latestValidHit = pattern;
      }
      else if(pattern > latestValidHit) latestValidHit = pattern;
    }

  }

  return latestValidHit;

}

/******************************************************************************/

const double
osu::Track::innerP () const
{
  double pInner = innerMomentum ().r ();
  if (this->matchedGsfTrack_.isNonnull ())
    pInner = this->matchedGsfTrack_->innerMomentum ().r ();

  return pInner;
}

const double
osu::Track::outerP () const
{
  double pInner = outerMomentum ().r ();
  if (this->matchedGsfTrack_.isNonnull ())
    pInner = this->matchedGsfTrack_->outerMomentum ().r ();

  return pInner;
}

const double
osu::Track::fbrem () const
{
  double pInner = this->innerP (),
         pOuter = this->outerP ();

  return (pInner ? max (0.0, pOuter - pInner) / pInner : 0.0);
}

const double
osu::Track::bremEnergy () const
{
  double pInner = this->innerP (),
         pOuter = this->outerP ();

  return max (0.0, pOuter - pInner);
}

const bool
osu::Track::isBadGsfTrack (const reco::GsfTrack &track) const
{
  bool passes = (track.pt () > 20.0)
             && (track.numberOfValidHits () >= 7)
             && (track.hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::MISSING_INNER_HITS) == 0)
             && (track.hitPattern ().trackerLayersWithoutMeasurement (reco::HitPattern::TRACK_HITS) <= 1)
             && (track.normalizedChi2 () < this->normalizedChi2 () + 1.0);

  return !passes;
}

int
osu::Track::isCloseToBadEcalChannel (const double &deltaRCut)
{
   double trackEta = this->eta(), trackPhi = this->phi();

   double min_dist = 999;
   DetId min_detId;

   std::map<DetId, std::vector<int> >::const_iterator bitItor;
   for(bitItor = EcalAllDeadChannelsBitMap_->begin(); bitItor != EcalAllDeadChannelsBitMap_->end(); bitItor++){

      DetId maskedDetId = bitItor->first;

      std::map<DetId, std::vector<double> >::const_iterator valItor = EcalAllDeadChannelsValMap_->find(maskedDetId);
      if( valItor == EcalAllDeadChannelsValMap_->end() ){ std::cout<<"Error cannot find maskedDetId in EcalAllDeadChannelsValMap_ ?!"<<std::endl; continue; }

      double eta = (valItor->second)[0], phi = (valItor->second)[1];

      double dist = reco::deltaR(eta, phi, trackEta, trackPhi);

      if( min_dist > dist ){ min_dist = dist; min_detId = maskedDetId; }
   }

   if( min_dist > deltaRCut && deltaRCut >0 ) return 0;

   return 1;
}

const bool
osu::Track::inTOBCrack () const
{
  return (fabs (this->dz ()) < 0.5 && fabs (this->lambda ()) < 1.0e-3);
}

#endif
