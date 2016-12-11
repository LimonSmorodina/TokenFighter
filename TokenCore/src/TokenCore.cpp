#include "TokenCore.hpp"

#include <random>
#include <algorithm>
#include <functional>

LinkTypesList TokenChainRing::getLinkTypes() const {
	LinkTypesList typesList;

	for each (TokenLink link in _nextRings)
	{
		typesList.push_back(link.second);
	}

	return typesList;
}

TokenChainLinkType reverseLinkDirection(TokenChainLinkType linkType) {
	TokenChainLinkType reverseLinkDir;
	switch (linkType) {
	case (UtoDDirectionLink): reverseLinkDir = DtoUDirectionLink; break;
	case (DtoUDirectionLink): reverseLinkDir = UtoDDirectionLink; break;
	case (LtoRDirectionLink): reverseLinkDir = RtoLDirectionLink; break;
	case (RtoLDirectionLink): reverseLinkDir = LtoRDirectionLink; break;
	}
	return reverseLinkDir;
}

TokenChainRingSP TokenChainRing::findRingWithToken(TokenSP tokenSP) const {
	TokenChainRingSP foundedRingSP;
	for each (TokenLink link in _nextRings)
	{
		if (link.first->correspondsTo(tokenSP)) return link.first;
		foundedRingSP = link.first->findRingWithToken(tokenSP);
		if (foundedRingSP != nullptr) return foundedRingSP;
	}
	return foundedRingSP;
}

TokenChainRingSP TokenChain::findRingWithToken(TokenSP tokenSP) const {
	_firstRingSP->findRingWithToken(tokenSP);
}

void TokenChain::addRing(TokenSP fTokenSP, TokenSP sTokenSP, TokenChainLinkType linkType) {
	// pointers to rings in chain that corresponds to added tokens
	TokenChainRingSP fTokenRingSP, sTokenRingSP;

	fTokenRingSP = findRingWithToken(fTokenSP);
	sTokenRingSP = findRingWithToken(sTokenSP);

	if (fTokenRingSP != nullptr) {
		if (sTokenRingSP != nullptr) {
			fTokenRingSP->addLink(sTokenRingSP, linkType);
		}
		else {
			if (_firstRingSP == fTokenRingSP) {
				_firstRingSP = TokenChainRingSP(new TokenChainRing(sTokenSP));
				_firstRingSP->addLink(fTokenRingSP, reverseLinkDirection(linkType));
			}
			else {
				fTokenRingSP->addLink(TokenChainRingSP(new TokenChainRing(sTokenSP)), linkType);
			}

			_ringCount++;
		}
	}
	else {
		// error?
	}
}

TokenChainLinkType orientateLink(TokenChainLinkType fLink, TokenChainLinkType sLink) {
    const static std::vector<TokenChainLinkType> clockWise = 
        {DtoUDirectionLink, LtoRDirectionLink, UtoDDirectionLink, RtoLdirectionLink};

	if (fLink == sLink) return StrictLink;
    // search in clockwise
}

void TokenChain::orientateLinks() {
    std::list<TokenLinkList> orientedLinks; // TokenLink

    auto firstLink = *_firstRingSP->getLinks()->begin();

    auto recursiveOrientate = [&orientedLinks](TokenLinkType prevLinkType, TokenLinkList links) {
        orientedLinks.push_back(LinkList());
        auto lastListIt = orientedLinks.end() - 1;
        for each (TokenLink link in links)
        {
            lastListIt->push_back(TokenLink(link.first, orientateLink(prevLinkType, link.second)));
            recOrientedLinks(link.second, link.first->getLinks();
        }
    };

    recursiveOriente(NoLink, _firstRingSP->getLinks);

    for each (TokenLinkList linkList in orientedLinks) {

    }
}

TokenField::TokenField(int xSize, int ySize) {
	_field.resize(ySize);
	for (auto it = _field.begin(); it != _field.end(); it++) {
		it->resize(xSize);
	}

	_chainSeekerP = new TokenChainSeeker(this);
}

void TokenField::fillWithRand() {
	for (auto tokenRow = _field.begin(); tokenRow != _field.end(); tokenRow++) {
		for (auto tokenSP = tokenRow->begin(); tokenSP != tokenRow->end(); tokenSP++) {
			if (*tokenSP == nullptr) {
				*tokenSP = TokenSP(new Token(static_cast<TokenType>(rand() % TokenTypeCount)));
			}
		}
	}
}

TokenChainSeeker::TokenChainSeeker(const TokenField* tokenField) {
	_field.resize(tokenField->getYsize());
	for (auto it = _field.begin(); it != _field.end(); it++) {
		it->resize(tokenField->getXSize());
	}

	for (int row = 0; row < _field.size(); row++) {
		for (int column = 0; column < _field.cbegin()->size(); column++) {
			_field[row][column].tokenSPP = *tokenField[row][column];
		}
	}
}

void TokenChainSeeker::resetCellParams() {
	for (int row = 0; row < _field.size(); row++) {
		for (int column = 0; column < _field.cbegin()->size(); column++) {
			_field[row][column].isProceed = false;
			_field[row][column].inChain = nullptr;
		}
	}
}

TokenChainUPListUP TokenChainSeeker::seekChains() {
    TokenChainUPListUP chainListUP = TokenChainUPListUP (new std::list <TokenChainUP>);

	resetCellParams();

    for (int row = 0; row < _field.size(); row++) {
        for (int column = 0; column < _field.begin()->size(); column++) {
            if (!_field[row][column].isProceed) {
				chainListUP->push_back(trailChain(row, column));
            }
        }
    }

    return chainListUP;
}

TokenChainUP TokenChainSeeker::trailChain(int row, int column) {
	TokenChainUP tokenChainUP = TokenChainUP(new TokenChain(*_field[row][column].tokenSPP));

	std::function<void (int, int)> recursiveTrail = [this, &tokenChainUP](int row, int column) {
		_field[row][column].inChain = tokenChainUP.get();
		_field[row][column].isProceed = true;

		if (row > 0 && (*_field[row - 1][column].tokenSPP)->getType() == tokenChainUP->getType()) {
			tokenChainUP->addRing(*_field[row][column].tokenSPP, *_field[row - 1][column].tokenSPP, DtoUDirectionLink);
		}
		if (column < _field[row].size() && (*_field[row][column + 1].tokenSPP)->getType() == tokenChainUP->getType()) {
			tokenChainUP->addRing(*_field[row][column].tokenSPP, *_field[row][column + 1].tokenSPP, LtoRDirectionLink);
		}
		if (row < _field.size() && (*_field[row + 1][column].tokenSPP)->getType() == tokenChainUP->getType()) {
			tokenChainUP->addRing(*_field[row][column].tokenSPP, *_field[row + 1][column].tokenSPP, UtoDDirectionLink);
		}
		if (column > 0 && (*_field[row][column - 1].tokenSPP)->getType() == tokenChainUP->getType()) {
			tokenChainUP->addRing(*_field[row][column].tokenSPP, *_field[row][column - 1].tokenSPP, RtoLDirectionLink);
		}
	};

	recursiveTrail(row, column);

	return tokenChainUP;
}
