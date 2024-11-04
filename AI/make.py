import os
import chromadb
from chromadb.utils import embedding_functions
from langchain_community.document_loaders import WebBaseLoader
from langchain.text_splitter import RecursiveCharacterTextSplitter
import requests
import json
import sseclient

class MeshyAPI:
    def __init__(self, api_key, base_url):
        self.base_url = base_url
        self.headers = {
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json"
        }
    
    def text_to_3d_preview(self, prompt):
        endpoint = "/v2/text-to-3d"  # v2 API 사용
        url = self.base_url + endpoint
        
        data = {
            "mode": "preview",  # preview 모드
            "prompt": prompt,
            "art_style": "realistic",
            "negative_prompt": "low quality, low resolution"
        }
        
        response = requests.post(url, headers=self.headers, json=data)
        return response.json()

    def text_to_3d_refine(self, preview_task_id):
        endpoint = "/v2/text-to-3d"
        url = self.base_url + endpoint
        
        data = {
            "mode": "refine",
            "preview_task_id": preview_task_id,
            "texture_richness": "high"
        }
        
        response = requests.post(url, headers=self.headers, json=data)
        return response.json()

    def get_task_status(self, task_id):
        endpoint = f"/v2/text-to-3d/{task_id}"
        url = self.base_url + endpoint
        
        response = requests.get(url, headers=self.headers)
        return response.json()

class ChatAPI:
    def __init__(self, api_key, base_url):
        self.api_key = api_key
        self.base_url = base_url
        self.headers = {
            'Content-Type': 'application/json',
            'cache-control': 'no-cache'
        }

    def create_chat_request(self, user_message, system_message):
        return {
            "app_id": "",
            "name": "",
            "item": [""],
            "param": [{
                "utterances": [
                    {
                        "role": "ROLE_SYSTEM",
                        "content": system_message
                    },
                    {
                        "role": "ROLE_USER",
                        "content": user_message
                    }
                ],
                "config": {
                    "top_p": 0.6,
                    "top_k": 1,
                    "temperature": 0.9,
                    "presence_penalty": 0.0,
                    "frequency_penalty": 0.0,
                    "repetition_penalty": 1.0
                },
                "stream": True
            }]
        }

    def stream_chat(self, user_message, system_message):
        request_data = self.create_chat_request(user_message, system_message)
        full_response = ""
        
        try:
            response = requests.post(
                self.base_url,
                headers=self.headers,
                json=request_data,
                stream=True
            )
            response.raise_for_status()
        
            for line in response.iter_lines():
                if line:
                    text = line.decode('utf-8')
                    full_response += text + "\n"
                
            return full_response
        
        except requests.exceptions.RequestException as e:
            print(f"Error during API request: {e}")
            return None

class ChromaRAGSystem:
    def __init__(self, api_key, base_url, collection_name="web_documents"):
        """ChromaDB 기반 RAG 시스템 초기화"""
        # Chat API 초기화
        self.chat_api = ChatAPI(api_key, base_url)
        
        # ChromaDB 클라이언트 초기화
        self.client = chromadb.PersistentClient(path="./chroma_db")
        
        # 임베딩 함수 설정 (Sentence Transformers 사용)
        self.embedding_function = embedding_functions.SentenceTransformerEmbeddingFunction(
            model_name="sentence-transformers/all-MiniLM-L6-v2"
        )
        
        # 컬렉션 생성 또는 로드
        try:
            self.collection = self.client.get_collection(
                name=collection_name,
                embedding_function=self.embedding_function
            )
            print(f"기존 컬렉션 '{collection_name}' 로드됨")
        except:
            self.collection = self.client.create_collection(
                name=collection_name,
                embedding_function=self.embedding_function
            )
            print(f"새 컬렉션 '{collection_name}' 생성됨")
        
        # 텍스트 스플리터 설정
        self.text_splitter = RecursiveCharacterTextSplitter(
            chunk_size=1000,  # 청크 크기 증가
            chunk_overlap=200,  # 오버랩 증가
            length_function=len,
            separators=["\n\n", "\n", ".", "!", "?", ",", " "]  # 구분자 추가
        )
    
    def load_and_process_url(self, url):
        import uuid
        import time
        try:
            # 기존 컬렉션 삭제 후 새로 생성
            collection_name = self.collection.name
            self.client.delete_collection(collection_name)
            
            # 새 컬렉션 생성
            self.collection = self.client.create_collection(
                name=collection_name,
                embedding_function=embedding_functions.SentenceTransformerEmbeddingFunction(
                    model_name="sentence-transformers/all-MiniLM-L6-v2"
                )
            )
            
            # 웹페이지 로드
            loader = WebBaseLoader(url)
            documents = loader.load()
            
            # 텍스트 분할
            chunks = self.text_splitter.split_documents(documents)
            
            # UUID를 사용한 고유 ID 생성
            ids = [str(uuid.uuid4()) for _ in range(len(chunks))]
            texts = [chunk.page_content for chunk in chunks]
            metadatas = [{"source": url, "timestamp": str(time.time())} for _ in chunks]
            
            # 새 데이터 추가
            self.collection.add(
                documents=texts,
                ids=ids,
                metadatas=metadatas
            )
            
            print(f"처리된 청크 수: {len(chunks)}")
            print(f"저장된 문서 수: {self.collection.count()}")
            
            return len(chunks)
            
        except Exception as e:
            print(f"Error during processing: {str(e)}")
            raise

    def ask_question(self, question, n_results=5):
        try:
            # 검색 전 디버깅 정보 출력
            print(f"\n검색 쿼리: {question}")
            print(f"데이터베이스 문서 수: {self.collection.count()}")
            
            # 관련 문서 검색
            results = self.collection.query(
                query_texts=[question],
                n_results=n_results
            )
            
            # 검색 결과 확인
            documents = results['documents'][0]
            print(f"\n검색된 문서 수: {len(documents)}")
            
            if documents:
                # 컨텍스트 구성
                context = "\n\n".join(documents)
                print(f"컨텍스트 길이: {len(context)} 문자")
                
                # 시스템 메시지 구성
                system_message = f"""아래의 컨텍스트만을 사용하여 골렘의 생김새와 외형적 특징에 대해 상세히 설명해주세요.
    컨텍스트에서 찾을 수 없는 정보는 포함하지 마세요.

    컨텍스트:
    {context}

    질문: {question}

    답변:"""
                
                # ChatAPI를 통한 답변 생성
                response = self.chat_api.stream_chat(question, system_message)
                return response if response else "답변을 생성하는 중 오류가 발생했습니다."
            else:
                return "관련 문서를 찾을 수 없습니다."
                
        except Exception as e:
            print(f"Error during question answering: {str(e)}")
            raise
        
def main():
    maal_key = ""
    maum_url = ""
    mesh_url = ""
    mesh_key = ""
    
    chat_api = ChatAPI(maal_key, maum_url)
    meshy = MeshyAPI(mesh_key, mesh_url)
    
    
    user_message = "진흙 골렘"
    system_message = " 생김새에 대해서만 말해줘"
    print("사용자:", user_message + system_message)
    print("AI 응답:", end=" ")
    
    
    try:
    # RAG 시스템 초기화
        rag_system = ChromaRAGSystem(maal_key, maum_url)
        
        # 웹페이지 로드 및 처리
        url = ""
        num_chunks = rag_system.load_and_process_url(url)
        print(f"웹페이지가 {num_chunks}개의 청크로 분할되었습니다.")
        
        # 질문하기
        question = user_message + system_message
            
        print("\n답변 생성 중...")
        
        # 1. MaumGPT로부터 설명 받기
        maal_text = rag_system.ask_question(question)
        print(f"\n답변:\n{maal_text}")
        
        
        # 2. Meshy로 3D 모델 생성
        preview_result = meshy.text_to_3d_preview(maal_text)
        preview_task_id = preview_result.get('result')
        
        # 3. Preview 작업 완료 대기
        import time
        while True:
            status = meshy.get_task_status(preview_task_id)
            if status.get('status') == 'SUCCEEDED':
                break
            elif status.get('status') == 'FAILED':
                print("Preview 생성 실패:", status.get('task_error', {}).get('message'))
                return
            time.sleep(5)  # 5초마다 상태 체크
        
        # 4. Refine 작업 시작
        refine_result = meshy.text_to_3d_refine(preview_task_id)
        refine_task_id = refine_result.get('result')
        
        # 5. Refine 작업 완료 대기
        while True:
            status = meshy.get_task_status(refine_task_id)
            if status.get('status') == 'SUCCEEDED':
                print("\n3D 모델 생성 완료!")
                print("다운로드 URL:")
                for format, url in status.get('model_urls', {}).items():
                    print(f"{format}: {url}")
                break
            elif status.get('status') == 'FAILED':
                print("Refine 생성 실패:", status.get('task_error', {}).get('message'))
                return
            time.sleep(5)
        
    except Exception as e:
        print(f"An error occurred: {str(e)}")
    
    


if __name__ == "__main__":
    main()
    # rigging
    os.system('python quick_start.py')
    
