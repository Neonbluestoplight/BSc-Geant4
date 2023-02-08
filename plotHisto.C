// ROOT macro file for plotting example B4 histograms
//
// Can be run from ROOT session:
// root[0] .x plotHisto.C

{
   gROOT->Reset();
   gROOT->SetStyle("Plain");
 
   // Draw histos filled by Geant4 simulation 
   // Open file filled by Geant4 simulation
   TFile *f = new TFile("B4.root","read");
   TTree *t = (TTree*)f->Get("B4");

   double total       = B4->GetEntries();
   double detected    = B4->GetEntries("Ediode>0");
   auto efficiency = 100*detected / total;

   // Create Canvas and divide it into 2 rows
   TCanvas *c = new TCanvas("c", "c", 800,800);
   c->Divide(1,2);


   //-----------ROW 1----------------------
   c->cd(1);
   // create upper pad
   TPad *p1 = new TPad("p1", //name
                       "p1", //title
                       0.,   //xlow	0-1 is the X position of the bottom left point of the pad
                       0.,   //ylow 0-1 is the Y position of the bottom left point of the pad
                       1.0,  //xup 0-1 is the X position of the top right point of the pad
                       1.0); //yup 0-1 is the X position of the top right point of the pad
   
   // Divide and draw upper pad into 2 columns
   p1->Divide(2,1);
   p1->Draw();

   // Draw Ediode histogram in the upper pad 1
   p1->cd(1);
   TH1D* hist1 = (TH1D*)f.Get("Ediode");
   hist1->Draw("HIST");
   
   // Draw Ldiode histogram in the upper pad 2
   p1->cd(2);
   TH1D* hist2 = (TH1D*)f.Get("Ldiode");
   hist2->Draw("HIST");
   //--------------------------------------

   //-----------ROW 2----------------------
   c->cd(2);
   // create lower pad
   TPad *p2 = new TPad("p2",  //name
                       "p2",  //title
                       0.,    //xlow	0-1 is the X position of the bottom left point of the pad
                       0.,    //ylow 0-1 is the Y position of the bottom left point of the pad
                       1.0,   //xup 0-1 is the X position of the top right point of the pad
                       1.0);  //yup 0-1 is the X position of the top right point of the pad

   // Divide and draw lower pad into 3 columns
   p2->Divide(3,1);
   p2->Draw();

   // Draw diodeX histogram in the lower pad 1
   p2->cd(1);
   TH1D* hist3 = (TH1D*)f.Get("diodeX");
   hist3->Draw("HIST");

   // Draw diodeY histogram in the lower pad 2
   p2->cd(2);
   TH1D* hist4 = (TH1D*)f.Get("diodeY");
   hist4->Draw("HIST");

   // Draw diodeZ histogram in the lower pad 3
   p2->cd(3);
   TH1D* hist5 = (TH1D*)f.Get("diodeZ");
   hist5->Draw("HIST");   
   //--------------------------------------
}