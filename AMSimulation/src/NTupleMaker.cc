#include "SLHCL1TrackTriggerSimulations/AMSimulation/interface/NTupleMaker.h"


// _____________________________________________________________________________
int NTupleMaker::readRoads(TString src) {
    if (src.EndsWith(".root")) {
        if (verbose_)  std::cout << Info() << "Opening " << src << std::endl;
        if (chain_roads_->Add(src) )  // 1 if successful, 0 otherwise
            return 0;
    }

    std::cout << Error() << "Roads source should be a .root file." << std::endl;
    return 1;
}


// _____________________________________________________________________________
int NTupleMaker::readTracks(TString src) {
    if (src.EndsWith(".root")) {
        if (verbose_)  std::cout << Info() << "Opening " << src << std::endl;
        if (chain_tracks_->Add(src) )  // 1 if successful, 0 otherwise
            return 0;
    }

    std::cout << Error() << "Tracks source should be a .root file." << std::endl;
    return 1;
}


// _____________________________________________________________________________
// Read the input ntuples
int NTupleMaker::readFile(TString src) {
    if (src.EndsWith(".root")) {
        if (verbose_)  std::cout << Info() << "Opening " << src << std::endl;
        if (chain_->Add(src) )  // 1 if successful, 0 otherwise
            return 0;

    } else if (src.EndsWith(".txt")) {
        TFileCollection* fc = new TFileCollection("fileinfolist", "", src);
        if (chain_->AddFileInfoList((TCollection*) fc->GetList()) )  // 1 if successful, 0 otherwise
            return 0;
    }

    std::cout << Error() << "Input source should be either a .root file or a .txt file." << std::endl;
    return 1;
}


// _____________________________________________________________________________
// Merge roads, tracks into main ntuple
int NTupleMaker::writeTree(TString out) {
    if (!out.EndsWith(".root")) {
        std::cout << Error() << "Output filename must be .root" << std::endl;
        return 1;
    }

    if (verbose_)  std::cout << Info() << "Reading " << nEvents_ << " events" << std::endl;

    // For writing
    TFile* tfile = TFile::Open(out, "RECREATE");
    tfile->mkdir("ntupler")->cd();

    if (trim_) {
        chain_->SetBranchStatus("TTClusters_*"   , 0);
        chain_->SetBranchStatus("simPixelDigis_*", 0);
        chain_->SetBranchStatus("simBeamSpot_*"  , 0);
    }

    TTree* ttree = (TTree*) chain_->CloneTree(0); // Do not copy the data yet
    // The clone should not delete any shared i/o buffers.
    ResetDeleteBranches(ttree);

    // For merging
    TObjArray* branches_roads = chain_roads_->GetListOfBranches();
    for (int i=0; i<branches_roads->GetEntries(); ++i) {
        TBranch* branch = (TBranch*) branches_roads->At(i);
        makeConnector(branch, ttree);
        connectors.back()->connect(chain_roads_);
    }

    //TObjArray* branches_tracks = chain_tracks_->GetListOfBranches();
    //for (int i=0; i<branches_tracks->GetEntries(); ++i) {
    //    TBranch* branch = (TBranch*) branches_tracks->At(i);
    //    makeConnector(branch, ttree);
    //    connectors.back()->connect(chain_tracks_);
    //}

    // _________________________________________________________________________
    // Loop over all events
    for (long long ievt=0; ievt<nEvents_; ++ievt) {
        Long64_t local_entry = chain_->LoadTree(ievt);  // for TChain
        if (local_entry < 0)  break;
        chain_->GetEntry(ievt);

        assert(chain_roads_->LoadTree(ievt) >= 0);
        chain_roads_->GetEntry(ievt);

        //assert(chain_tracks_->LoadTree(ievt) >= 0);
        //chain_tracks_->GetEntry(ievt);

        if (verbose_>1 && ievt%10000==0)  std::cout << Debug() << Form("... Writing event: %7lld", ievt) << std::endl;

        ttree->Fill();
    }

    tfile->Write();
    delete ttree;
    delete tfile;
    return 0;
}


// _____________________________________________________________________________
// Private functions

// TypedBranchConnector constructor
template <class T>
NTupleMaker::TypedBranchConnector<T>::TypedBranchConnector(TString s, TString t, TTree * tree)
: instanceName_(s) {
    ptr_object_ = &object_;
    if (t != "") {
        tree->Branch(s, ptr_object_, s+"/"+t);  // raw type -- not implemented
    } else {
        tree->Branch(s, &ptr_object_);  // STL type
    }
}

// TypedBranchConnector set branch address
template <class T>
void NTupleMaker::TypedBranchConnector<T>::connect(TTree* tree) {
    tree->SetBranchAddress(instanceName_, &ptr_object_);
}

void NTupleMaker::makeLeafMap() {
    leafmap["char"] = CHAR_T;
    leafmap["unsigned char"] = UCHAR_T;
    leafmap["short"] = SHORT_T;
    leafmap["unsigned short"] = USHORT_T;
    leafmap["int"] = INT_T;
    leafmap["unsigned int"] = UINT_T;
    leafmap["float"] = FLOAT_T;
    leafmap["double"] = DOUBLE_T;
    leafmap["Long64_t"] = LONG64_T;
    leafmap["ULong64_t"] = ULONG64_T;

    leafmap["vector<char>"] = CHAR_V;
    leafmap["vector<unsigned char>"] = UCHAR_V;
    leafmap["vector<short>"] = SHORT_V;
    leafmap["vector<unsigned short>"] = USHORT_V;
    leafmap["vector<int>"] = INT_V;
    leafmap["vector<unsigned int>"] = UINT_V;
    leafmap["vector<float>"] = FLOAT_V;
    leafmap["vector<double>"] = DOUBLE_V;
    leafmap["vector<Long64_t>"] = LONG64_V;
    leafmap["vector<ULong64_t>"] = ULONG64_V;
    leafmap["vector<bool>"] = BOOL_V;

    leafmap["vector<vector<char> >"] = CHAR_VV;
    leafmap["vector<vector<unsigned char> >"] = UCHAR_VV;
    leafmap["vector<vector<short> >"] = SHORT_VV;
    leafmap["vector<vector<unsigned short> >"] = USHORT_VV;
    leafmap["vector<vector<int> >"] = INT_VV;
    leafmap["vector<vector<unsigned int> >"] = UINT_VV;
    leafmap["vector<vector<float> >"] = FLOAT_VV;
    leafmap["vector<vector<double> >"] = DOUBLE_VV;
    leafmap["vector<vector<Long64_t> >"] = LONG64_VV;
    leafmap["vector<vector<ULong64_t> >"] = ULONG64_VV;
    leafmap["vector<vector<bool> >"] = BOOL_VV;
}

void NTupleMaker::makeConnector(const TBranch* branch, TTree* tree) {
    TString branchName = branch->GetName();
    TString branchClassName = branch->GetClassName();

    // Determine the leaf type
    std::map<TString, LeafType>::iterator found = leafmap.find(branchClassName);
    switch (found->second) {
        case CHAR_T    : connectors.push_back(new TypedBranchConnector<Char_t>    (branchName, "B", tree) ); break;
        case UCHAR_T   : connectors.push_back(new TypedBranchConnector<UChar_t>   (branchName, "b", tree) ); break;
        case SHORT_T   : connectors.push_back(new TypedBranchConnector<Short_t>   (branchName, "S", tree) ); break;
        case USHORT_T  : connectors.push_back(new TypedBranchConnector<UShort_t>  (branchName, "s", tree) ); break;
        case INT_T     : connectors.push_back(new TypedBranchConnector<Int_t>     (branchName, "I", tree) ); break;
        case UINT_T    : connectors.push_back(new TypedBranchConnector<UInt_t>    (branchName, "i", tree) ); break;
        case FLOAT_T   : connectors.push_back(new TypedBranchConnector<Float_t>   (branchName, "F", tree) ); break;
        case DOUBLE_T  : connectors.push_back(new TypedBranchConnector<Double_t>  (branchName, "D", tree) ); break;
        case LONG64_T  : connectors.push_back(new TypedBranchConnector<Long64_t>  (branchName, "L", tree) ); break;
        case ULONG64_T : connectors.push_back(new TypedBranchConnector<ULong64_t> (branchName, "l", tree) ); break;
        case BOOL_T    : connectors.push_back(new TypedBranchConnector<Bool_t>    (branchName, "O", tree) ); break;

        case CHAR_V    : connectors.push_back(new TypedBranchConnector<std::vector<Char_t> >    (branchName, "", tree) ); break;
        case UCHAR_V   : connectors.push_back(new TypedBranchConnector<std::vector<UChar_t> >   (branchName, "", tree) ); break;
        case SHORT_V   : connectors.push_back(new TypedBranchConnector<std::vector<Short_t> >   (branchName, "", tree) ); break;
        case USHORT_V  : connectors.push_back(new TypedBranchConnector<std::vector<UShort_t> >  (branchName, "", tree) ); break;
        case INT_V     : connectors.push_back(new TypedBranchConnector<std::vector<Int_t> >     (branchName, "", tree) ); break;
        case UINT_V    : connectors.push_back(new TypedBranchConnector<std::vector<UInt_t> >    (branchName, "", tree) ); break;
        case FLOAT_V   : connectors.push_back(new TypedBranchConnector<std::vector<Float_t> >   (branchName, "", tree) ); break;
        case DOUBLE_V  : connectors.push_back(new TypedBranchConnector<std::vector<Double_t> >  (branchName, "", tree) ); break;
        case LONG64_V  : connectors.push_back(new TypedBranchConnector<std::vector<Long64_t> >  (branchName, "", tree) ); break;
        case ULONG64_V : connectors.push_back(new TypedBranchConnector<std::vector<ULong64_t> > (branchName, "", tree) ); break;
        case BOOL_V    : connectors.push_back(new TypedBranchConnector<std::vector<Bool_t> >    (branchName, "", tree) ); break;

        case CHAR_VV   : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Char_t> > >    (branchName, "", tree) ); break;
        case UCHAR_VV  : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<UChar_t> > >   (branchName, "", tree) ); break;
        case SHORT_VV  : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Short_t> > >   (branchName, "", tree) ); break;
        case USHORT_VV : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<UShort_t> > >  (branchName, "", tree) ); break;
        case INT_VV    : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Int_t> > >     (branchName, "", tree) ); break;
        case UINT_VV   : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<UInt_t> > >    (branchName, "", tree) ); break;
        case FLOAT_VV  : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Float_t> > >   (branchName, "", tree) ); break;
        case DOUBLE_VV : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Double_t> > >  (branchName, "", tree) ); break;
        case LONG64_VV : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Long64_t> > >  (branchName, "", tree) ); break;
        case ULONG64_VV: connectors.push_back(new TypedBranchConnector<std::vector<std::vector<ULong64_t> > > (branchName, "", tree) ); break;
        case BOOL_VV   : connectors.push_back(new TypedBranchConnector<std::vector<std::vector<Bool_t> > >    (branchName, "", tree) ); break;

        default       : std::cout << Error() << "Cannot handle leaf of ClassName: " << branchClassName << std::endl; break;
    }
}


// _____________________________________________________________________________
// Main driver
int NTupleMaker::run(TString out, TString src, TString roadfile, TString trackfile) {
    gROOT->ProcessLine("#include <vector>");  // how is it not loaded?

    int exitcode = 0;
    Timing(1);

    exitcode = readRoads(roadfile);
    if (exitcode)  return exitcode;
    Timing();

    //exitcode = readTracks(trackfile);
    //if (exitcode)  return exitcode;
    //Timing();

    exitcode = readFile(src);
    if (exitcode)  return exitcode;
    Timing();

    exitcode = writeTree(out);
    if (exitcode)  return exitcode;
    Timing();

    chain_->Reset();
    chain_roads_->Reset();
    chain_tracks_->Reset();
    return exitcode;
}
