// ROOT macro file for plotting example B4 histograms
// Can be run from ROOT session: root[0] .x plotHisto.C
// Draw histos filled by Geant4 simulation & open file filled by Geant4 simulation

void plotHisto()
{  
   gROOT-> Reset();
   gROOT-> SetStyle("Plain");
   
   TFile *f = new TFile("B4.root","read");
   TTree *t = (TTree*)f->Get("B4");   

   double total           = t->GetEntries();               // Number of particles emitted by source
   double diodeDetected   = t->GetEntries("Ediode>0");     // Number of particles detected by diode
   double annularDetected = t->GetEntries("Eannular>0");   // Number of particles detected by diode

   auto diodeEfficiency = 100*diodeDetected/total;     // Efficiency = (Number detected/Total number)x100 %
   auto diodeError      = (1/std::sqrt(diodeDetected))*diodeEfficiency; // Error in efficiency
   auto diodeFracError  = diodeError/diodeEfficiency;

   auto annularEfficiency = 100*annularDetected/total;     // Efficiency of annular
   auto annularError      = (1/std::sqrt(annularDetected))*annularEfficiency; // Error in efficiency
   auto annularFracError  = annularError/annularEfficiency;

   auto collectiveEfficiency = diodeEfficiency+annularEfficiency; // Collective efficiency
   auto collectiveFracError  = std::sqrt((diodeFracError*diodeFracError)+(annularFracError*annularFracError));
   auto collectiveError      = collectiveEfficiency*collectiveFracError; // Collective error

   double totalDetected   = t->GetEntries("Ediode==5");    // Number of particles depositing full energy
   auto partialDetected   = diodeDetected - totalDetected; // Number of particles not depositing full energy
   auto partialPercent    = (partialDetected/totalDetected)*100; // Percentage of partial depositors

   cout<<"Efficiency of PIN diode = "<<diodeEfficiency<<"+/-"<<diodeError<<endl;
   cout<<"Efficiency of Annular detector = "<<annularEfficiency<<"+/-"<<annularError<<endl;
   cout<<"Collective efficiency = "<<collectiveEfficiency<<"+/-"<<collectiveError<<endl;
   cout<<"percentage of partial depositors = "<<partialPercent<<" percent"<<endl; 

   std::ofstream outfile1;
   outfile1.open("diode_efficiency_data.dat", std::ios_base::app);
   outfile1 << diodeEfficiency << "," <<diodeError << "\n";
   outfile1.close();

   std::ofstream outfile2;
   outfile2.open("annular_efficiency_data.dat", std::ios_base::app);
   outfile2 << annularEfficiency << "," <<annularError << "\n";
   outfile2.close();

   std::ofstream outfile3;
   outfile3.open("collective_efficiency_data.dat", std::ios_base::app);
   outfile3 << collectiveEfficiency << "," <<collectiveError << "\n";
   outfile3.close();

   TCanvas *c1 = new TCanvas();
   TH1D* enHist = (TH1D*)f->Get("Ediode");
   gPad->SetLogy(1);
   enHist->Draw("HIST");
   enHist->GetXaxis()->SetRangeUser(-1,5.1);
   enHist->GetXaxis()->SetTickLength(-0.01);
   enHist->GetXaxis()->SetTitle("Energy (MeV)");
   enHist->GetYaxis()->SetTickLength(-0.01);  
   enHist->GetYaxis()->SetTitle("Intensity (counts)");

/* TCanvas *c2 = new TCanvas();
   c2->Divide(3,1);

   TH1F *histx = new TH1F("x-component", "x momentum component", 1000, -1.5, 1.5);
   TH1F *histy = new TH1F("y-component", "y momentum component", 1000, -1.5, 1.5);
   TH1F *histz = new TH1F("z-component", "z momentum component", 1000, -1.5, 1.5);

   double x,y,z;
   fstream datafile;
   datafile.open("data.dat", ios::in);
   while(1)
   {
      datafile>>x>>y>>z;
      histx->Fill(x);
      histy->Fill(y);
      histz->Fill(z);
      if(datafile.eof()) break;
   }
   datafile.close();

   c2->cd(1);
   histx->Draw();
   c2->cd(2);
   histy->Draw();
   c2->cd(3);
   histz->Draw();*/
}
