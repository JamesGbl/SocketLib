#ifndef __2014_03_02_CIOBUFFER_H__
#define __2014_03_02_CIOBUFFER_H__

#include "CObject.h"
#include "MemoryPool.h"

class IOBlock : public AllocFromMemoryPool, public NoCopyable
{
public:
	enum	{MAX_BLOCK_SIZE = 8192 }; //block ��Ĵ�Сֵ byteΪ��λ
	IOBlock()
	{
		reset();
	}
	~IOBlock()
	{

	}
	void	lock()
	{
		m_mutex.lock();
	}
	void	unlock()
	{
		m_mutex.unlock();
	}
	/*
	*	��ȡIOBlock�е�����
	*	@para	pBuffer: �洢��ȡ�����ݣ�����ΪNULL��Ҳ���Ǵ˴ζ�ȡֻ��Ϊ��ɾ��Block�е�����
	*	@para	nLenght: Ҫ��ȡ���ݵĳ���
	*	@para	bDel   : �Ƿ��Block��ɾ����ȡ������
	*	@return:	�ɹ���ȡ�����ݳ��ȣ����pBufferΪNULL��˳���Ϊ��ȡ���ȵĸ���
	*/
	int	readData(void * pBuffer, int nLength, bool bDel = true)
	{
		if(nLength <= 0)	return 0;

		int nReadAbleSize = m_nWriteIndex - m_nReadIndex;

		if(nLength > nReadAbleSize )
			nLength = nReadAbleSize;

		if(NULL != pBuffer)
			memcpy(pBuffer, &m_pBuffer[m_nReadIndex], nLength);
		if(bDel)
			m_nReadIndex += nLength;

		return (NULL == pBuffer) ? - nLength : nLength;
	}
	/*
	*	��IOBlock��д������
	*	@para	pBuffer: Ҫд������ݣ�����ΪNULL��Ҳ���Ǵ˴�д��ֻ��Ϊ�˴�block��Ԥ��һ�οռ�
	*	@para	nLenght: Ҫд�����ݵĳ���
	*	@return:	�ɹ�д������ݳ��ȣ����pBufferΪNULL��˳���Ϊд�볤�ȵĸ���
	*/
	int	writeData(void* pBuffer, int nLength)
	{
		if(nLength <= 0)  return 0;

		int nWriteAbleSize = MAX_BLOCK_SIZE - m_nWriteIndex;
		if(nLength > nWriteAbleSize)
			nLength = nWriteAbleSize;

		if(NULL != pBuffer)
			memcpy(&m_pBuffer[m_nWriteIndex], pBuffer, nLength);
		m_nWriteIndex += nLength;
		
		return (NULL == pBuffer) ? -nLength : nLength;
	}
	/*
	*	��ȡ��ǰBlock��δ��ʹ�õ�buffer
	*	@para	pBuffer: ָ�����buffer���׵�ַ
	*	@para	nLenght: ���ÿռ��size
	*	@return:	�п��ÿռ䷵��true�����򷵻�false
	*/
	bool	getEmptyBuffer(void *&pBuffer, int &nDataSize)
	{
		nDataSize = MAX_BLOCK_SIZE - m_nWriteIndex;
		if(nDataSize <= 0) return false;
		pBuffer = (void*)&m_pBuffer[m_nWriteIndex];

		return true;
	}
	/*
	*	��ȡ��ǰblock������д���buffer����
	*	@return:	���ؿ�д��ĳ���
	*/
	int		getWriteAbleSize()
	{
		return MAX_BLOCK_SIZE - m_nWriteIndex;
	}
	/*
	*	��ȡ��ǰblock�е��Ѿ�д�����ݵ�buffer
	*	@para	pBuffer: block��д�����ݵ�buffer�׵�ַ
	*	@para	nDataSize: block���Ѿ�д�����ݵ�buffer��size
	*	@return:	block����д�����ݷ���true�����򷵻�false
	*/
	bool	getDataBuffer(void *&pBuffer, int &nDataSize)
	{
		nDataSize = m_nWriteIndex - m_nReadIndex;
		if(nDataSize <= 0) return false;
		pBuffer = (void*)&m_pBuffer[m_nReadIndex];
		return true;
	}
	/*
	*	��ȡ��ǰblock�еĿ��Զ�ȡ������size
	*	@return:	block�пɶ�ȡ���ݵ�size
	*/
	int		getReadAbleSize()
	{
		return m_nWriteIndex - m_nReadIndex;
	}
	/*
	*	����
	*/
	void	reset()
	{
		m_nWriteIndex = 0;
		m_nReadIndex = 0;
	}
private:
	Mutex			m_mutex;
	char			m_pBuffer[MAX_BLOCK_SIZE];
	int				m_nWriteIndex; //block ��ǰ�Ŀ�д��λ��
	int				m_nReadIndex;  //block ��ǰ�Ŀɶ�ȡλ��
};

class IOBuffer : public NoCopyable
{
public:
	typedef		IOBlock							IOBlockTemplate;
	typedef		std::vector<IOBlockTemplate*>	IOBufferQueue;

	IOBuffer()
	{
		m_pCurBlock = NULL;
		m_nDataSize = 0;
	}

	virtual ~IOBuffer()
	{
		clearIOBlocks();
	}
	void lock()
	{
		m_mutex.lock();
	}
	void unlock()
	{
		m_mutex.unlock();
	}
	int readData(void* pBuffer, int nLength, bool bDel = true)
	{
		if( nLength <= 0 ) return 0;

		void* pDataBuf = pBuffer;
		int   nReadLen = 0;

		if(nLength > m_nDataSize)
			nLength = m_nDataSize;

		IOBufferQueue::iterator itBlock = m_IoBlocks.begin();
		while( itBlock != m_IoBlocks.end() )
		{
			IOBlockTemplate* pBlock = *itBlock;

			assert(NULL != pBlock);

			if( (NULL != pBlock) )
			{
				int nSize = pBlock->readData(pDataBuf, nLength - nReadLen, bDel);
				if(NULL != pDataBuf)
					pDataBuf = (char*)pDataBuf + nSize;
				else
					nSize *= -1;

				nReadLen += nSize;
				if(bDel)
					m_nDataSize -= nSize;

				if(nReadLen >= nLength) break;

				if(bDel)
				{
					itBlock = m_IoBlocks.erase(itBlock);
					if(m_pCurBlock == pBlock)
						m_pCurBlock = NULL;
					releaseBlock(pBlock);
				}
				else
					itBlock++;
			}
		}
		return NULL == pBuffer ? -nReadLen : nReadLen;
	}
	bool getDataBuffer(void *&pBuffer, int &nDataSize)
	{
		pBuffer = NULL; nDataSize = 0;

		IOBufferQueue::iterator itBlock = m_IoBlocks.begin();
		while( itBlock != m_IoBlocks.end() )
		{
			IOBlockTemplate *pBlock = *itBlock;
			assert(NULL != pBlock);
			if( pBlock->getDataBuffer(pBuffer, nDataSize) )
				return true;

			itBlock = m_IoBlocks.erase(itBlock);
			if(pBlock == m_pCurBlock)
				m_pCurBlock = NULL;
			releaseBlock(pBlock);
		}
		return false;
	}

	bool getEmptyBuffer(void *&pBuffer, int &nDataSize, bool bNewBlock = false)
	{
		pBuffer = NULL; nDataSize = 0;

		if(bNewBlock)
			m_pCurBlock = createBlock();
		else
		{
			if(NULL == m_pCurBlock || m_pCurBlock->getWriteAbleSize() <= 0)
				m_pCurBlock = createBlock();
		}

		return m_pCurBlock->getEmptyBuffer(pBuffer, nDataSize);
	}
	int writeData(void* pBuffer, int nLength)
	{
		if( nLength <= 0 ) return 0;

		void* pDataBuf = pBuffer;
		int   nWriteLen = 0;

		if( NULL == m_pCurBlock)
			m_pCurBlock = createBlock();

		while(1)
		{
			int nSize = m_pCurBlock->writeData(pDataBuf, nLength - nWriteLen);
			if(NULL != pDataBuf)
				pDataBuf  = (char*)pDataBuf + nSize;
			else
				nSize *= -1;

			nWriteLen += nSize;
			m_nDataSize += nSize;

			if(nWriteLen >= nLength) break;

			m_pCurBlock = createBlock();
		}

		return NULL == pBuffer ? -nWriteLen : nWriteLen;
	}

	int	 getDataSize()
	{
		return m_nDataSize;
	}
private:
	void clearIOBlocks()
	{
		IOBufferQueue::iterator it = m_IoBlocks.begin();
		for(; it != m_IoBlocks.end(); it++)
		{
			IOBlockTemplate *pBlock = *it;
			releaseBlock(pBlock);
		}
		m_IoBlocks.clear();		
	}
	void	releaseBlock(IOBlockTemplate *&pBlock)
	{
		SAFE_DELETE(pBlock);
	}
	IOBlockTemplate* createBlock()
	{
		IOBlockTemplate* pNewBlock = new IOBlockTemplate;
		m_IoBlocks.push_back(pNewBlock);
		return pNewBlock;
	}
private:
	IOBufferQueue		m_IoBlocks;
	int					m_nDataSize;
	IOBlockTemplate	   *m_pCurBlock;
	Mutex				m_mutex;
};
#endif//__2014_03_02_CIOBUFFER_H__