#pragma once

#include <memory>
#include <list>
#include <vector>

struct Position2 {
	int x, y;
};

typedef Position2 TokenPosition;

enum TokenType{
	Token1 = 0, 
	Token2, 
	TokenTypeCount
};

class Token
{
public:
	Token(TokenType type) : _type(type) {}

	bool operator== (Token& token) { return this->_type == token._type; }

	TokenType getType() { return _type; }
private:
	TokenType _type;
};

typedef std::shared_ptr<Token> TokenSP;
typedef std::unique_ptr<Token> TokenUP;
typedef std::unique_ptr< std::list<TokenSP> > TokenSPListUP;

enum TokenChainLinkType {
    RtoLDirectionLink,
    LtoRDirectionLink,
    UtoDDirectionLink,
    DtoUDirectionLink,
	StrictLink,
	CWTurnLink,
	CCWTurnLink
};

typedef std::list<TokenChainLinkType> LinkTypesList;

TokenChainLinkType reverseLinkDirection(TokenChainLinkType);

class TokenChainRing
{
public:
	explicit TokenChainRing(TokenSP tokenSP): _tokenSP(tokenSP) {}
	~TokenChainRing() {}

	TokenSP getTokenSP() const { return _tokenSP; }

	void addLink(TokenChainRingSP nextRing, TokenChainLinkType linkType) 
	{ _nextRings.push_back(TokenLink(nextRing, linkType)); }

	bool correspondsTo (TokenSP tokenSP) const { return _tokenSP == tokenSP; }

	TokenChainRingSP findRingWithToken(TokenSP tokenSP) const;

	LinkTypesList getLinkTypes() const;
private:
	typedef std::pair<TokenChainRingSP, TokenChainLinkType> TokenLink;

	TokenSP _tokenSP;
	std::list< TokenLink > _nextRings;
};

typedef std::shared_ptr< TokenChainRing > TokenChainRingSP;

class TokenChain
{
public:
	explicit TokenChain(TokenSP firsrTokenSP) :
		_chainType(firsrTokenSP->getType()), _firstRingSP(new TokenChainRing(firsrTokenSP)), _ringCount(0) {}

	TokenType getType() const { return  _chainType; }
	int getRingsCount() const { return  _ringCount; }
	
	void addRing(TokenSP, TokenSP, TokenChainLinkType);

	void orientateLinks();
private:
	TokenChainRingSP findRingWithToken(TokenSP) const;

	TokenType _chainType;
	int _ringCount;
	TokenChainRingSP _firstRingSP;
};

typedef std::shared_ptr< TokenChain > TokenChainSP;
typedef std::unique_ptr< TokenChain > TokenChainUP;
typedef std::unique_ptr< std::list<TokenChainUP> > TokenChainUPListUP;

class TokenField
{
public:
	TokenField(int xSize, int ySize);

	void fillWithRand();

	int getXSize() const { return _field.cbegin()->size(); }
	int getYsize() const { return _field.size(); }

private:
	std::vector< std::vector<TokenSP> > _field;
	TokenChainSeeker* _chainSeekerP;
};

class TokenChainSeeker
{
public:
    TokenChainSeeker(const TokenField*);
    TokenChainUPListUP seekChains();

private:
    struct ChainSeekerCell {
        const TokenSP *tokenSPP; // :S
        bool isProceed;
        TokenChain *inChain;
    };

	void resetCellParams();

    TokenChainUP trailChain(int row, int column);

    std::vector< std::vector< ChainSeekerCell > > _field;
};

