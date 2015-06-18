import FWCore.ParameterSet.Config as cms

ntuplePixelDigis = cms.EDProducer('NTuplePixelDigis',
    inputTag = cms.InputTag('simSiPixelDigis'),
    inputTagTP = cms.InputTag('mix', 'MergedTrackTruth'),
    prefix = cms.string('pixelDigis@'),
    suffix = cms.string(''),
    cut = cms.string(''),
    maxN = cms.uint32(999999)
)

ntupleStubs = cms.EDProducer('NTupleStubs',
    inputTagClus = cms.InputTag('TTClustersFromPixelDigis', 'ClusterInclusive'),
    inputTagStub = cms.InputTag('TTStubsFromPixelDigis', 'StubAccepted'),
    inputTagTrack = cms.InputTag('TTTracksFromPixelDigis', 'Level1TTTracks'),
    inputTagDigi = cms.InputTag('simSiPixelDigis'),
    inputTagClusMCAssoc = cms.InputTag('TTClusterAssociatorFromPixelDigis', 'ClusterInclusive'),
    inputTagStubMCAssoc = cms.InputTag('TTStubAssociatorFromPixelDigis', 'StubAccepted'),
    inputTagTrackMCAssoc = cms.InputTag('TTTrackAssociatorFromPixelDigis', 'Level1TTTracks'),
    prefixClus = cms.string('TTClusters2@'),
    prefixStub = cms.string('TTStubs2@'),
    prefixTrack = cms.string('TTTracks@'),
    prefixDigi = cms.string('simPixelDigis@'),
    suffix = cms.string(''),
    #cut = cms.string(''),
    #maxN = cms.uint32(999999)
)

ntupleStubsTTI = ntupleStubs.clone(
    inputTagDigi = cms.InputTag(''),
    inputTagClus = cms.InputTag('TTStubsFromPixelDigis', 'ClusterAccepted'),
    inputTagClusMCAssoc = cms.InputTag('TTClusterAssociatorFromPixelDigis', 'ClusterAccepted'),
)

ntupleTTClusters = cms.EDProducer('NTupleTTClusters',
    inputTag = cms.InputTag('TTClustersFromPixelDigis', 'ClusterInclusive'),
    inputTagMC = cms.InputTag('TTClusterAssociatorFromPixelDigis', 'ClusterInclusive'),
    prefix = cms.string('TTClusters@'),
    suffix = cms.string(''),
    cut = cms.string(''),
    maxN = cms.uint32(999999)
)

ntupleTTStubs = cms.EDProducer('NTupleTTStubs',
    inputTag = cms.InputTag('TTStubsFromPixelDigis', 'StubAccepted'),
    inputTagMC = cms.InputTag('TTStubAssociatorFromPixelDigis', 'StubAccepted'),
    prefix = cms.string('TTStubs@'),
    suffix = cms.string(''),
    cut = cms.string(''),
    maxN = cms.uint32(999999)
)

ntupleTTTracks = cms.EDProducer('NTupleTTTracks',
    inputTag = cms.InputTag('TTTracksFromPixelDigis', 'Level1TTTracks'),
    inputTagMC = cms.InputTag('TTTrackAssociatorFromPixelDigis', 'Level1TTTracks'),
    nparameters = cms.int32(4),
    prefix = cms.string('TrackletTTTracks@'),
    suffix = cms.string(''),
    cut = cms.string(''),
    maxN = cms.uint32(999999)
)

ntupleL1TrackTrigger = cms.Sequence(ntuplePixelDigis * ntupleStubs * ntupleTTClusters * ntupleTTStubs * ntupleTTTracks)
ntupleL1TrackTrigger_TTI = cms.Sequence(ntuplePixelDigis * ntupleStubsTTI * ntupleTTClusters * ntupleTTStubs * ntupleTTTracks)

