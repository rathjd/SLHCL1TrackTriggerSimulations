#!/usr/bin/env python

from rootdrawing import *
from parser import *
import pdgName

col  = TColor.GetColor("#e31a1c")  # nu140
fcol = TColor.GetColor("#fb9a99")  # nu140
#col  = TColor.GetColor("#6a3d9a")  # tttt140
#fcol = TColor.GetColor("#cab2d6")  # tttt140

c_pdgName = pdgName.pdgName()


# ______________________________________________________________________________
def drawer_book():
    histos = {}

    hname = "trkParts_pt"
    histos[hname] = TH1F(hname, "; trkPart p_{T} [GeV]", 200, 0, 100)

    hname = "trkParts_invPt"
    histos[hname] = TH1F(hname, "; trkPart 1/p_{T} [1/GeV]", 220, -5.5, 5.5)

    hname = "trkParts_eta"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} #eta", 300, -5, 5)

    hname = "trkParts_phi"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} #phi [#pi rad]", 220, -1.1, 1.1)

    hname = "trkParts_vx"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} vertex x [cm]", 200, -0.01, 0.01)

    hname = "trkParts_vy"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} vertex y [cm]", 200, -0.01, 0.01)

    hname = "trkParts_vz"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} vertex z [cm]", 200, -30, 30)

    hname = "trkParts_charge"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} charge", 5, -2.5, 2.5)

    hname = "trkParts_pdgName"
    histos[hname] = TH1F(hname, "; trkPart_{pt>2} pdgName", 50, -25, 25)

    hname = "ntrkParts_pt0p5"
    histos[hname] = TH1F(hname, "; # trkParts {pt>0.5}", 250, 0, 2500)

    hname = "ntrkParts_pt1"
    histos[hname] = TH1F(hname, "; # trkParts {pt>1}", 250, 0, 1000)

    hname = "ntrkParts_pt2"
    histos[hname] = TH1F(hname, "; # trkParts {pt>2}", 250, 0, 250)

    hname = "ntrkParts_pt3"
    histos[hname] = TH1F(hname, "; # trkParts {pt>3}", 250, 0, 250)

    hname = "ntrkParts_pt5"
    histos[hname] = TH1F(hname, "; # trkParts {pt>5}", 250, 0, 250)

    hname = "ntrkParts_pt10"
    histos[hname] = TH1F(hname, "; # trkParts {pt>10}", 250, 0, 250)

    hname = "nvertices"
    histos[hname] = TH1F(hname, "; # vertices", 250, 0, 250)

    # Style
    for hname, h in histos.iteritems():
        h.SetLineWidth(2); h.SetMarkerSize(0)
        h.SetLineColor(col); h.SetFillColor(fcol)
    donotdelete.append(histos)
    return histos

def drawer_project(tree, histos, options):
    tree.SetBranchStatus("*", 0)
    tree.SetBranchStatus("trkParts_pt"     , 1)
    tree.SetBranchStatus("trkParts_eta"    , 1)
    tree.SetBranchStatus("trkParts_phi"    , 1)
    tree.SetBranchStatus("trkParts_vx"     , 1)
    tree.SetBranchStatus("trkParts_vy"     , 1)
    tree.SetBranchStatus("trkParts_vz"     , 1)
    tree.SetBranchStatus("trkParts_charge" , 1)
    tree.SetBranchStatus("trkParts_pdgId"  , 1)
    tree.SetBranchStatus("trkParts_primary", 1)
    tree.SetBranchStatus("gen_nPV"         , 1)

    # Loop over events
    for ievt, evt in enumerate(tree):
        if (ievt == options.nentries):  break
        if (ievt % 100 == 0):  print "Processing event: %i" % ievt

        nparts = evt.trkParts_pt.size()
        pt_map = {}

        for ipart in xrange(nparts):
            primary = evt.trkParts_primary[ipart]
            charge  = evt.trkParts_charge [ipart]

            if not (charge!=0 and primary):
                continue

            pt      = evt.trkParts_pt     [ipart]
            eta     = evt.trkParts_eta    [ipart]
            phi     = evt.trkParts_phi    [ipart]
            vx      = evt.trkParts_vx     [ipart]
            vy      = evt.trkParts_vy     [ipart]
            vz      = evt.trkParts_vz     [ipart]
            charge  = evt.trkParts_charge [ipart]
            pdgId   = evt.trkParts_pdgId  [ipart]

            if pt > 2:  # relax eta cut
                histos["trkParts_eta"    ].Fill(eta)

            if abs(eta) < 2.4:  # relax pt cut
                histos["trkParts_pt"     ].Fill(pt)
                histos["trkParts_invPt"  ].Fill(charge/pt)

                pt_map[ipart] = pt

            if not (pt > 2 and abs(eta) < 2.4):
                continue

            histos["trkParts_phi"    ].Fill(phi/pi)
            histos["trkParts_vx"     ].Fill(vx)
            histos["trkParts_vy"     ].Fill(vy)
            histos["trkParts_vz"     ].Fill(vz)
            histos["trkParts_charge" ].Fill(charge)
            histos["trkParts_pdgName"].Fill(c_pdgName.get(pdgId), 1)

        histos["ntrkParts_pt0p5" ].Fill(count_if(pt_map.values(), lambda x: x>0.5))
        histos["ntrkParts_pt1"   ].Fill(count_if(pt_map.values(), lambda x: x>1))
        histos["ntrkParts_pt2"   ].Fill(count_if(pt_map.values(), lambda x: x>2))
        histos["ntrkParts_pt3"   ].Fill(count_if(pt_map.values(), lambda x: x>3))
        histos["ntrkParts_pt5"   ].Fill(count_if(pt_map.values(), lambda x: x>5))
        histos["ntrkParts_pt10"  ].Fill(count_if(pt_map.values(), lambda x: x>10))

        histos["nvertices"       ].Fill(evt.gen_nPV)

    tree.SetBranchStatus("*", 1)
    return

def drawer_draw(histos, options):
    for hname, h in histos.iteritems():
        if hname in ["trkParts_pt"]:
            options.logy = True
        else:
            options.logy = False

        if hname == "trkParts_pt":
            h.SetMaximum(h.GetMaximum() / 100.)

        if options.logy:
            h.SetMaximum(h.GetMaximum() * 14); h.SetMinimum(0.5)
        else:
            h.SetMaximum(h.GetMaximum() * 1.4); h.SetMinimum(0.)

        if hname == "trkParts_vz":
            h.Fit("gaus", "N")

        if hname == "trkParts_pdgName":
            h.LabelsDeflate("X")
            h.LabelsOption("a")

        h.Draw("hist")
        gPad.SetLogy(options.logy)

        CMS_label()
        save(options.outdir, hname)

    return

def drawer_sitrep(histos, options):
    print "--- SITREP ---------------------------------------------------------"

    h = histos["trkParts_pdgName"]
    bincontents = {}
    integral = 0
    for i in xrange(1,h.GetNbinsX()+1):
        #print i, h.GetXaxis().GetBinLabel(i), h.GetBinContent(i)
        binlabel = h.GetXaxis().GetBinLabel(i).replace("~","").replace("^+","^+-").replace("^-","^+-")
        bincontents[binlabel] = bincontents.get(binlabel,0) + h.GetBinContent(i)
        integral += h.GetBinContent(i)
    assert(integral == h.Integral())

    print "Primary tracking particle composition:"
    for k, v in sorted(bincontents.items(), key=lambda x: x[1], reverse=True):
        print "{0:8s} {1:.3f}".format(k, float(v)/integral)


# ______________________________________________________________________________
# Main function
def main(options):

    # Init
    drawerInit = DrawerInit()
    tchain = TChain("ntupler/tree", "")
    tchain.AddFileInfoList(options.tfilecoll.GetList())

    # Process
    histos = drawer_book()
    drawer_project(tchain, histos, options)
    drawer_draw(histos, options)
    drawer_sitrep(histos, options)


# ______________________________________________________________________________
if __name__ == '__main__':

    # Setup argument parser
    parser = argparse.ArgumentParser()

    # Add default arguments
    add_drawer_arguments(parser)

    # Add more arguments
    #parser.add_argument("ss", help="short name of superstrip definition (e.g. ss256)")
    #parser.add_argument("npatterns", type=int, help="number of patterns to reach the desired coverage")

    # Parse default arguments
    options = parser.parse_args()
    parse_drawer_options(options)

    # Call the main function
    main(options)
