// ROOT macro file for plotting example B4 histograms
// Can be run from ROOT session: root[0] .x plotHisto.C
// Draw histos filled by Geant4 simulation & open file filled by Geant4 simulation

void plotHisto()
{  
   gROOT-> Reset();
   gROOT->SetStyle("Plain");
   
   TFile *f    = new TFile("B4.root","read");
   TTree *t    = (TTree*)f->Get("B4");   

   double total    = t->GetEntries();                    // Number of particles emitted by source
   double detected = t->GetEntries("Ediode>0");          // Number of particles detected by diode
   auto efficiency = 100*detected/total;                 // Efficiency = (Number detected/Total number)x100 %
   auto error      = (1/std::sqrt(detected))*efficiency; // Error in efficiency

   cout<<"Efficiency = "<<efficiency<<"+/-"<<error<<endl;

   std::ofstream outfile;
   outfile.open("efficiency_data.dat", std::ios_base::app);
   outfile << efficiency << "+/-" <<error << "\n";
   outfile.close();

/* TCanvas *c1 = new TCanvas();
   c1->Divide(3,1);

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

   c1->cd(1);
   histx->Draw();
   c1->cd(2);
   histy->Draw();
   c1->cd(3);
   histz->Draw();*/
}
