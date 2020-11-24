#ifndef SDHCALRunAction_h
#define SDHCALRunAction_h

#include <globals.hh>
#include <G4UserRunAction.hh>

#include "SDHCALLcioWriter.h"
#include "SDHCALRootWriter.h"

#include <string>


class G4Run ;

class SDHCALRunAction : public G4UserRunAction
{
	public :
		SDHCALRunAction() = default;

		SDHCALRunAction(G4String fileName) ;
		virtual ~SDHCALRunAction() ;

		G4Run* GenerateRun() ;

		virtual void BeginOfRunAction(const G4Run*) ;
		virtual void EndOfRunAction(const G4Run*) ;

		inline SDHCALLcioWriter* getWriter() const { return writer ; }

		inline void setLcioFileName(G4String name) { lcioFileName = name ; }
		inline void setRootFileName(G4String name) { rootFileName = name ; }

		static void setCreateGlobleWriter() { createGlobalWriter = true; }

		SDHCALRunAction(const SDHCALRunAction&) = delete ;
		void operator=(const SDHCALRunAction&) = delete ;

	protected :
		SDHCALLcioWriter* writer = nullptr ;

		G4String lcioFileName = "test.slcio" ;
		G4String rootFileName = "test.root" ;

	private:
		static bool createGlobalWriter ;
} ;

#endif //SDHCALRunAction_h

